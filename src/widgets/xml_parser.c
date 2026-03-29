#include "widgets/xml_parser.h"

#include "widgets/button.h"
#include "widgets/container.h"
#include "widgets/display.h"
#include "widgets/input.h"
#include "widgets/menu.h"
#include "widgets/separator.h"
#include "widgets/toggle.h"
#include "widgets/window.h"
#include "widgets/date.h"
#include "widgets/common.h"

#include <string.h>
#include <stdlib.h>

/* ── Max nesting depth for the widget tree ── */
#define MAX_DEPTH 32

/* ── ID registry (flat table, filled during parse) ── */
#define MAX_IDS 128

typedef struct {
    const char *id;
    GtkWidget  *widget;
} id_entry;

static id_entry  g_id_table[MAX_IDS];
static int        g_id_count = 0;

static void register_id(const char *id, GtkWidget *widget) {
    if (!id || g_id_count >= MAX_IDS) return;
    g_id_table[g_id_count].id     = g_strdup(id);
    g_id_table[g_id_count].widget = widget;
    g_id_count++;
}

GtkWidget *xml_get_widget_by_id(const char *id) {
    for (int i = 0; i < g_id_count; i++) {
        if (g_strcmp0(g_id_table[i].id, id) == 0)
            return g_id_table[i].widget;
    }
    return NULL;
}

/* ── Internal parser state ── */
typedef struct {
    const xml_parse_context *ctx;

    GtkWidget *stack[MAX_DEPTH];
    int        depth;

    GtkWidget *result;       /* final top-level widget */

    /* For dropdown options memory management */
    GPtrArray *string_arrays;

    /* Menu parsing state for <menubar> trees */
    struct xml_menubar_state *menu_state;
} parser_state;

typedef struct xml_menu_item_node {
    char *label;
    bool enabled;
    menu_item_callback on_activate;
    gpointer user_data;
    GPtrArray *children; /* xml_menu_item_node* */
} xml_menu_item_node;

typedef struct {
    char *label;
    GPtrArray *items; /* xml_menu_item_node* */
} xml_menu_section_node;

typedef struct xml_menubar_state {
    GPtrArray *sections; /* xml_menu_section_node* */
    GPtrArray *item_stack; /* xml_menu_item_node* */
    xml_menu_section_node *current_section;
    widget_style_config style;
    int layout_col;
    int layout_row;
    int layout_colspan;
    int layout_rowspan;
    menu_layout_mode layout;
    bool show_arrow_indicators;
} xml_menubar_state;

/* ── Helpers ── */

static const char *find_attr(const gchar **names, const gchar **values,
                             const char *key) {
    for (int i = 0; names[i]; i++) {
        if (g_strcmp0(names[i], key) == 0)
            return values[i];
    }
    return NULL;
}

static int attr_int(const gchar **names, const gchar **values,
                    const char *key, int fallback) {
    const char *v = find_attr(names, values, key);
    return v ? atoi(v) : fallback;
}

static double attr_double(const gchar **names, const gchar **values,
                          const char *key, double fallback) {
    const char *v = find_attr(names, values, key);
    return v ? atof(v) : fallback;
}

static bool attr_bool(const gchar **names, const gchar **values,
                      const char *key, bool fallback) {
    const char *v = find_attr(names, values, key);
    if (!v) return fallback;
    return (g_strcmp0(v, "true") == 0 || g_strcmp0(v, "1") == 0);
}

static GtkOrientation attr_orientation(const gchar **names,
                                       const gchar **values,
                                       const char *key,
                                       GtkOrientation fallback) {
    const char *v = find_attr(names, values, key);
    if (!v) return fallback;
    if (g_strcmp0(v, "horizontal") == 0) return GTK_ORIENTATION_HORIZONTAL;
    if (g_strcmp0(v, "vertical") == 0)   return GTK_ORIENTATION_VERTICAL;
    return fallback;
}

static GtkAlign attr_align(const gchar **names,
                           const gchar **values,
                           const char *key,
                           GtkAlign fallback) {
    const char *v = find_attr(names, values, key);
    if (!v) return fallback;
    if (g_strcmp0(v, "start") == 0) return GTK_ALIGN_START;
    if (g_strcmp0(v, "center") == 0) return GTK_ALIGN_CENTER;
    if (g_strcmp0(v, "end") == 0) return GTK_ALIGN_END;
    if (g_strcmp0(v, "fill") == 0) return GTK_ALIGN_FILL;
    return fallback;
}

/* Parse the style sub-attributes from the element attributes. */
static widget_style_config parse_style(const gchar **names,
                                       const gchar **values) {
    widget_style_config s = {0};
    s.margin_top    = attr_int(names, values, "style-margin-top", 0);
    s.margin_bottom = attr_int(names, values, "style-margin-bottom", 0);
    s.margin_start  = attr_int(names, values, "style-margin-start", 0);
    s.margin_end    = attr_int(names, values, "style-margin-end", 0);
    s.width_request  = attr_int(names, values, "style-width", 0);
    s.height_request = attr_int(names, values, "style-height", 0);
    
    if (find_attr(names, values, "style-halign")) {
        s.halign = attr_align(names, values, "style-halign", GTK_ALIGN_FILL);
        s.set_halign = true;
    }
    if (find_attr(names, values, "style-valign")) {
        s.valign = attr_align(names, values, "style-valign", GTK_ALIGN_FILL);
        s.set_valign = true;
    }

    if (find_attr(names, values, "style-hexpand")) {
        s.hexpand = attr_bool(names, values, "style-hexpand", false);
        s.set_hexpand = true;
    }
    if (find_attr(names, values, "style-vexpand")) {
        s.vexpand = attr_bool(names, values, "style-vexpand", false);
        s.set_vexpand = true;
    }

    const char *css = find_attr(names, values, "style-css-class");
    if (css) s.css_class = css;

    const char *tip = find_attr(names, values, "tooltip");
    if (tip) s.tooltip = tip;

    return s;
}

/* Look up a callback by name from the registry */
static void *lookup_callback(const parser_state *ps, const char *name) {
    if (!name || !ps->ctx->callbacks) return NULL;
    for (size_t i = 0; i < ps->ctx->callback_count; i++) {
        if (g_strcmp0(ps->ctx->callbacks[i].name, name) == 0)
            return ps->ctx->callbacks[i].func;
    }
    g_warning("xml_parser: callback '%s' not found in registry", name);
    return NULL;
}

/* Push a container onto the stack so children get added to it */
static void push(parser_state *ps, GtkWidget *widget) {
    if (ps->depth >= MAX_DEPTH) {
        g_warning("xml_parser: max nesting depth exceeded");
        return;
    }
    ps->stack[ps->depth++] = widget;
}

/* Current parent container (top of stack) */
static GtkWidget *current_parent(parser_state *ps) {
    return ps->depth > 0 ? ps->stack[ps->depth - 1] : NULL;
}

static void add_to_parent_at(parser_state *ps, GtkWidget *widget,
                             int col, int row, int colspan, int rowspan) {
    GtkWidget *parent = current_parent(ps);
    if (parent) {
        container_add(parent, widget, col, row, colspan, rowspan);
    }
}

/* Add a newly-created widget to the current parent container */
static void add_to_parent(parser_state *ps, GtkWidget *widget, 
                          const gchar **names, const gchar **values) {
    int col = attr_int(names, values, "layout-col", 0);
    int row = attr_int(names, values, "layout-row", 0);
    int colspan = attr_int(names, values, "layout-colspan", 1);
    int rowspan = attr_int(names, values, "layout-rowspan", 1);
    add_to_parent_at(ps, widget, col, row, colspan, rowspan);
}

static xml_menu_item_node *menu_item_node_new(void) {
    xml_menu_item_node *node = g_new0(xml_menu_item_node, 1);
    node->enabled = true;
    node->children = g_ptr_array_new();
    return node;
}

static void menu_item_node_free(xml_menu_item_node *node) {
    if (!node) return;
    for (guint i = 0; i < node->children->len; i++) {
        menu_item_node_free(g_ptr_array_index(node->children, i));
    }
    g_ptr_array_free(node->children, TRUE);
    g_free(node->label);
    g_free(node);
}

static xml_menu_section_node *menu_section_node_new(void) {
    xml_menu_section_node *section = g_new0(xml_menu_section_node, 1);
    section->items = g_ptr_array_new();
    return section;
}

static void menu_section_node_free(xml_menu_section_node *section) {
    if (!section) return;
    for (guint i = 0; i < section->items->len; i++) {
        menu_item_node_free(g_ptr_array_index(section->items, i));
    }
    g_ptr_array_free(section->items, TRUE);
    g_free(section->label);
    g_free(section);
}

static xml_menubar_state *menubar_state_new(void) {
    xml_menubar_state *state = g_new0(xml_menubar_state, 1);
    state->sections = g_ptr_array_new();
    state->item_stack = g_ptr_array_new();
    state->layout_col = 0;
    state->layout_row = 0;
    state->layout_colspan = 1;
    state->layout_rowspan = 1;
    state->layout = MENU_LAYOUT_VERTICAL;
    state->show_arrow_indicators = true;
    return state;
}

static void menubar_state_free(xml_menubar_state *state) {
    if (!state) return;
    for (guint i = 0; i < state->sections->len; i++) {
        menu_section_node_free(g_ptr_array_index(state->sections, i));
    }
    g_ptr_array_free(state->sections, TRUE);
    g_ptr_array_free(state->item_stack, TRUE);
    g_free(state);
}

static void materialize_menu_item(const xml_menu_item_node *src,
                                  menu_item_config *dst,
                                  GPtrArray *allocations) {
    dst->label = src->label;
    dst->enabled = src->enabled;
    dst->on_activate = src->on_activate;
    dst->user_data = src->user_data;

    if (src->children->len > 0) {
        menu_item_config *children =
            g_new0(menu_item_config, src->children->len);
        g_ptr_array_add(allocations, children);
        dst->kind = MENU_ITEM_SUBMENU;
        dst->children = children;
        dst->child_count = src->children->len;
        dst->on_activate = NULL;
        dst->user_data = NULL;
        for (guint i = 0; i < src->children->len; i++) {
            materialize_menu_item(g_ptr_array_index(src->children, i),
                                  &children[i], allocations);
        }
    } else {
        dst->kind = MENU_ITEM_ACTION;
        dst->children = NULL;
        dst->child_count = 0;
    }
}

static GtkWidget *build_menubar_widget(parser_state *ps,
                                       const xml_menubar_state *state) {
    if (!state || state->sections->len == 0) {
        return NULL;
    }

    GPtrArray *allocations = g_ptr_array_new();
    menu_section_config *sections =
        g_new0(menu_section_config, state->sections->len);
    g_ptr_array_add(allocations, sections);

    for (guint i = 0; i < state->sections->len; i++) {
        xml_menu_section_node *src_section = g_ptr_array_index(state->sections, i);
        sections[i].label = src_section->label;
        sections[i].item_count = src_section->items->len;
        if (src_section->items->len > 0) {
            menu_item_config *items =
                g_new0(menu_item_config, src_section->items->len);
            g_ptr_array_add(allocations, items);
            sections[i].items = items;
            for (guint j = 0; j < src_section->items->len; j++) {
                materialize_menu_item(g_ptr_array_index(src_section->items, j),
                                      &items[j], allocations);
            }
        } else {
            sections[i].items = NULL;
        }
    }

    GtkWidget *menubar = create_menubar(&(menubar_config){
        .sections = sections,
        .section_count = state->sections->len,
        .action_map = G_ACTION_MAP(ps->ctx->app),
        .layout = state->layout,
        .show_arrow_indicators = state->show_arrow_indicators,
        .style = state->style,
    });

    for (guint i = 0; i < allocations->len; i++) {
        g_free(g_ptr_array_index(allocations, i));
    }
    g_ptr_array_free(allocations, TRUE);

    return menubar;
}

/* ── Element handlers ── */

static GtkWidget *handle_window(parser_state *ps,
                                const gchar **names, const gchar **values) {
    window_config cfg = {
        .title          = find_attr(names, values, "title"),
        .icon_name      = find_attr(names, values, "icon-name"),
        .widget_name    = find_attr(names, values, "widget-name"),
        .background_image_path = find_attr(names, values, "background-image"),
        .default_width  = attr_int(names, values, "default-width", 800),
        .default_height = attr_int(names, values, "default-height", 600),
        .min_width      = attr_int(names, values, "min-width", 0),
        .min_height     = attr_int(names, values, "min-height", 0),
        .max_width      = attr_int(names, values, "max-width", 0),
        .max_height     = attr_int(names, values, "max-height", 0),
        .resizable      = attr_bool(names, values, "resizable", true),
        .decorated      = attr_bool(names, values, "decorated", true),
        .modal          = attr_bool(names, values, "modal", false),
        .maximized      = attr_bool(names, values, "maximized", false),
        .present_on_create = false,
        .style          = parse_style(names, values),
    };
    return create_window(ps->ctx->app, &cfg);
}

static GtkWidget *handle_box(parser_state *ps,
                             const gchar **names, const gchar **values) {
    (void)ps;
    box_config cfg = {
        .orientation = attr_orientation(names, values, "orientation", GTK_ORIENTATION_VERTICAL),
        .spacing     = attr_int(names, values, "spacing", 0),
        .homogeneous = attr_bool(names, values, "homogeneous", false),
        .style       = parse_style(names, values),
        .align       = attr_align(names, values, "align", GTK_ALIGN_FILL),
    };
    return create_box(&cfg);
}

static GtkWidget *handle_grid(parser_state *ps,
                              const gchar **names, const gchar **values) {
    (void)ps;
    grid_config cfg = {
        .column_spacing       = attr_int(names, values, "column-spacing", 0),
        .row_spacing          = attr_int(names, values, "row-spacing", 0),
        .homogeneous_columns  = attr_bool(names, values, "homogeneous-columns", false),
        .homogeneous_rows     = attr_bool(names, values, "homogeneous-rows", false),
        .style                = parse_style(names, values),
    };
    return create_grid(&cfg);
}

static GtkWidget *handle_stack(parser_state *ps,
                               const gchar **names, const gchar **values) {
    (void)ps;
    
    const char *trans_str = find_attr(names, values, "transition");
    GtkStackTransitionType trans = GTK_STACK_TRANSITION_TYPE_SLIDE_UP_DOWN;
    if (g_strcmp0(trans_str, "none") == 0) trans = GTK_STACK_TRANSITION_TYPE_NONE;
    else if (g_strcmp0(trans_str, "crossfade") == 0) trans = GTK_STACK_TRANSITION_TYPE_CROSSFADE;
    else if (g_strcmp0(trans_str, "slide") == 0) trans = GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT;

    stack_config cfg = {
        .transition    = trans,
        .duration_ms   = (guint)attr_int(names, values, "duration-ms", 280),
        .vhomogeneous  = attr_bool(names, values, "vhomogeneous", false),
        .hhomogeneous  = attr_bool(names, values, "hhomogeneous", false),
        .style         = parse_style(names, values),
    };
    return create_stack(&cfg);
}

static GtkWidget *handle_stack_sidebar(parser_state *ps,
                               const gchar **names, const gchar **values) {
    const char *stack_id = find_attr(names, values, "stack-id");
    GtkWidget *stack = xml_get_widget_by_id(stack_id);
    if (!stack) {
        g_warning("xml_parser: stack-sidebar requires a valid stack-id");
        return NULL;
    }
    
    widget_style_config style = parse_style(names, values);
    return create_stack_sidebar(stack, &style);
}

static GtkWidget *handle_button(parser_state *ps,
                                const gchar **names, const gchar **values) {
    const char *cb_name = find_attr(names, values, "on-clicked");
    button_config cfg = {
        .label         = find_attr(names, values, "label"),
        .icon_name     = find_attr(names, values, "icon-name"),
        .theme_variant = find_attr(names, values, "theme-variant"),
        .use_underline = attr_bool(names, values, "use-underline", false),
        .has_frame     = attr_bool(names, values, "has-frame", true),
        .can_shrink    = attr_bool(names, values, "can-shrink", false),
        .icon_size     = attr_int(names, values, "icon-size", 0),
        .style         = parse_style(names, values),
        .on_clicked    = (xml_button_callback)lookup_callback(ps, cb_name),
        .user_data     = ps->ctx->user_data,
    };
    return create_button(&cfg);
}

static GtkWidget *handle_entry(parser_state *ps,
                               const gchar **names, const gchar **values) {
    const char *cb_name = find_attr(names, values, "on-changed");
    entry_config cfg = {
        .placeholder  = find_attr(names, values, "placeholder"),
        .default_text = find_attr(names, values, "default-text"),
        .purpose_hint = find_attr(names, values, "purpose-hint"),
        .password     = attr_bool(names, values, "password", false),
        .editable     = attr_bool(names, values, "editable", true),
        .max_length   = attr_int(names, values, "max-length", 0),
        .style        = parse_style(names, values),
        .on_changed   = (xml_entry_callback)lookup_callback(ps, cb_name),
        .user_data    = ps->ctx->user_data,
    };
    return create_entry(&cfg);
}

static GtkWidget *handle_spin_button(parser_state *ps,
                                     const gchar **names,
                                     const gchar **values) {
    const char *cb_name = find_attr(names, values, "on-value-changed");
    spin_button_config cfg = {
        .min_value    = attr_double(names, values, "min", 0),
        .max_value    = attr_double(names, values, "max", 100),
        .step         = attr_double(names, values, "step", 1),
        .value        = attr_double(names, values, "value", 0),
        .digits       = (guint)attr_int(names, values, "digits", 0),
        .numeric_only = attr_bool(names, values, "numeric-only", false),
        .wrap         = attr_bool(names, values, "wrap", false),
        .style        = parse_style(names, values),
        .on_value_changed = (xml_spin_callback)lookup_callback(ps, cb_name),
        .user_data    = ps->ctx->user_data,
    };
    return create_spin_button(&cfg);
}

static GtkWidget *handle_dropdown(parser_state *ps,
                                  const gchar **names, const gchar **values) {
    const char *cb_name = find_attr(names, values, "on-selected");
    const char *options_str = find_attr(names, values, "options");
    
    char **string_array = NULL;
    if (options_str) {
        string_array = g_strsplit(options_str, ",", -1);
        g_ptr_array_add(ps->string_arrays, string_array); // Schedule for cleanup
    }

    dropdown_config cfg = {
        .options = (const char **)string_array,
        .selected_index = (guint)attr_int(names, values, "selected-index", 0),
        .enable_search = attr_bool(names, values, "enable-search", false),
        .style = parse_style(names, values),
        .on_selected = (xml_dropdown_callback)lookup_callback(ps, cb_name),
        .user_data = ps->ctx->user_data,
    };
    return create_dropdown(&cfg);
}

static GtkWidget *handle_calendar(parser_state *ps,
                                  const gchar **names, const gchar **values) {
    const char *cb_name = find_attr(names, values, "on-day-selected");
    GDateTime *now = g_date_time_new_now_local(); // Use current date for simplicity by default
    
    calendar_config cfg = {
        .selected_date = now,
        .show_day_names = attr_bool(names, values, "show-day-names", true),
        .show_week_numbers = attr_bool(names, values, "show-week-numbers", false),
        .no_month_change = attr_bool(names, values, "no-month-change", false),
        .style = parse_style(names, values),
        .on_day_selected = (xml_calendar_callback)lookup_callback(ps, cb_name),
        .user_data = ps->ctx->user_data,
    };
    GtkWidget *cal = create_calendar(&cfg);
    g_date_time_unref(now);
    return cal;
}

static GtkWidget *handle_label(parser_state *ps,
                               const gchar **names, const gchar **values) {
    (void)ps;
    label_config cfg = {
        .text            = find_attr(names, values, "text"),
        .selectable      = attr_bool(names, values, "selectable", false),
        .wrap            = attr_bool(names, values, "wrap", false),
        .max_width_chars = attr_int(names, values, "max-width-chars", -1),
        .style           = parse_style(names, values),
        .ellipsize       = find_attr(names, values, "ellipsize") ? PANGO_ELLIPSIZE_END : PANGO_ELLIPSIZE_NONE,
    };
    return create_label(&cfg);
}

static GtkWidget *handle_image(parser_state *ps,
                               const gchar **names, const gchar **values) {
    (void)ps;
    image_config cfg = {
        .icon_name  = find_attr(names, values, "icon-name"),
        .file_path  = find_attr(names, values, "file-path"),
        .pixel_size = attr_int(names, values, "pixel-size", 0),
        .keep_aspect = attr_bool(names, values, "keep-aspect", true),
        .style       = parse_style(names, values),
    };
    return create_image(&cfg);
}

static GtkWidget *handle_progress_bar(parser_state *ps,
                                      const gchar **names,
                                      const gchar **values) {
    (void)ps;
    progress_bar_config cfg = {
        .fraction   = attr_double(names, values, "fraction", 0),
        .text       = find_attr(names, values, "text"),
        .show_text  = attr_bool(names, values, "show-text", false),
        .inverted   = attr_bool(names, values, "inverted", false),
        .style      = parse_style(names, values),
    };
    return create_progress_bar(&cfg);
}

static GtkWidget *handle_spinner(parser_state *ps,
                                 const gchar **names, const gchar **values) {
    (void)ps;
    spinner_config cfg = {
        .size     = attr_int(names, values, "size", 32),
        .spinning = attr_bool(names, values, "spinning", true),
        .style    = parse_style(names, values),
    };
    return create_spinner(&cfg);
}

static GtkWidget *handle_separator(parser_state *ps,
                                   const gchar **names,
                                   const gchar **values) {
    (void)ps;
    separator_config cfg = {
        .orientation = attr_orientation(names, values, "orientation",
                                        GTK_ORIENTATION_HORIZONTAL),
        .style       = parse_style(names, values),
    };
    return create_separator(&cfg);
}

static GtkWidget *handle_checkbox(parser_state *ps,
                                  const gchar **names,
                                  const gchar **values) {
    const char *cb_name = find_attr(names, values, "on-toggled");
    checkbox_config cfg = {
        .label        = find_attr(names, values, "label"),
        .active       = attr_bool(names, values, "active", false),
        .inconsistent = attr_bool(names, values, "inconsistent", false),
        .style        = parse_style(names, values),
        .on_toggled   = (xml_checkbox_callback)lookup_callback(ps, cb_name),
        .user_data    = ps->ctx->user_data,
    };
    return create_checkbox(&cfg);
}

static GtkWidget *handle_switch(parser_state *ps,
                                const gchar **names,
                                const gchar **values) {
    const char *cb_name = find_attr(names, values, "on-active-changed");
    const char *id = find_attr(names, values, "id");
    
    switch_config cfg = {
        .label  = find_attr(names, values, "label"),
        .active = attr_bool(names, values, "active", false),
        .state  = attr_bool(names, values, "state", false),
        .style  = parse_style(names, values),
        .on_active_changed = (xml_switch_callback)lookup_callback(ps, cb_name),
        .user_data         = ps->ctx->user_data,
    };
    
    GtkWidget *switch_widget = NULL;
    GtkWidget *row = create_switch_row(&cfg, &switch_widget);
    
    if (id && switch_widget) {
        /* Usually the id maps to the row, but if they want the inner switch they 
         * should use another id. For now we register the row. */
    }
    
    return row;
}

static GtkWidget *handle_radio_button(parser_state *ps,
                                      const gchar **names,
                                      const gchar **values) {
    const char *cb_name = find_attr(names, values, "on-toggled");
    const char *group_id = find_attr(names, values, "group-with");
    GtkWidget *group_widget = group_id ? xml_get_widget_by_id(group_id) : NULL;

    radio_button_config cfg = {
        .label  = find_attr(names, values, "label"),
        .group_with = group_widget,
        .is_active = attr_bool(names, values, "is-active", false),
        .style  = parse_style(names, values),
        .on_toggled = (void *)lookup_callback(ps, cb_name),
        .user_data         = ps->ctx->user_data,
    };
    return create_radio_button(&cfg);
}

/* ── GMarkupParser callbacks ── */

static void xml_start_element(GMarkupParseContext *context,
                              const gchar         *element_name,
                              const gchar        **attribute_names,
                              const gchar        **attribute_values,
                              gpointer             user_data,
                              GError             **error) {
    (void)context;
    (void)error;
    parser_state *ps = user_data;
    GtkWidget *widget = NULL;
    bool is_container = false;

    if (g_strcmp0(element_name, "menubar") == 0) {
        if (ps->menu_state != NULL) {
            g_warning("xml_parser: nested menubar is not supported");
            return;
        }
        ps->menu_state = menubar_state_new();
        ps->menu_state->style = parse_style(attribute_names, attribute_values);
        ps->menu_state->layout_col = attr_int(attribute_names, attribute_values,
                                              "layout-col", 0);
        ps->menu_state->layout_row = attr_int(attribute_names, attribute_values,
                                              "layout-row", 0);
        ps->menu_state->layout_colspan = attr_int(attribute_names, attribute_values,
                                                  "layout-colspan", 1);
        ps->menu_state->layout_rowspan = attr_int(attribute_names, attribute_values,
                                                  "layout-rowspan", 1);
        ps->menu_state->show_arrow_indicators =
            attr_bool(attribute_names, attribute_values,
                      "show-arrow-indicators", true);

        const char *layout = find_attr(attribute_names, attribute_values, "layout");
        if (g_strcmp0(layout, "horizontal") == 0) {
            ps->menu_state->layout = MENU_LAYOUT_HORIZONTAL;
        } else {
            ps->menu_state->layout = MENU_LAYOUT_VERTICAL;
        }
        return;
    }

    if (g_strcmp0(element_name, "menu-section") == 0) {
        if (!ps->menu_state) {
            g_warning("xml_parser: menu-section must be inside menubar");
            return;
        }
        xml_menu_section_node *section = menu_section_node_new();
        const char *label = find_attr(attribute_names, attribute_values, "label");
        section->label = g_strdup(label ? label : "");
        g_ptr_array_add(ps->menu_state->sections, section);
        ps->menu_state->current_section = section;
        return;
    }

    if (g_strcmp0(element_name, "menu-item") == 0) {
        if (!ps->menu_state) {
            g_warning("xml_parser: menu-item must be inside menubar");
            return;
        }
        xml_menu_item_node *item = menu_item_node_new();
        const char *label = find_attr(attribute_names, attribute_values, "label");
        const char *cb_name = find_attr(attribute_names, attribute_values, "on-activate");
        item->label = g_strdup(label ? label : "");
        item->enabled = attr_bool(attribute_names, attribute_values, "enabled", true);
        item->on_activate = (menu_item_callback)lookup_callback(ps, cb_name);
        item->user_data = ps->ctx->app;

        if (ps->menu_state->item_stack->len > 0) {
            xml_menu_item_node *parent = g_ptr_array_index(
                ps->menu_state->item_stack,
                ps->menu_state->item_stack->len - 1);
            g_ptr_array_add(parent->children, item);
        } else if (ps->menu_state->current_section) {
            g_ptr_array_add(ps->menu_state->current_section->items, item);
        } else {
            g_warning("xml_parser: menu-item must be inside menu-section");
            menu_item_node_free(item);
            return;
        }

        g_ptr_array_add(ps->menu_state->item_stack, item);
        return;
    }

    /* ── Containers (push onto stack) ── */
    if (g_strcmp0(element_name, "window") == 0) {
        widget = handle_window(ps, attribute_names, attribute_values);
        is_container = true;
    }
    else if (g_strcmp0(element_name, "box") == 0) {
        widget = handle_box(ps, attribute_names, attribute_values);
        is_container = true;
    }
    else if (g_strcmp0(element_name, "grid") == 0) {
        widget = handle_grid(ps, attribute_names, attribute_values);
        is_container = true;
    }
    else if (g_strcmp0(element_name, "stack") == 0) {
        widget = handle_stack(ps, attribute_names, attribute_values);
        is_container = true;
    }
    /* ── Leaf widgets ── */
    else if (g_strcmp0(element_name, "stack-sidebar") == 0) {
        widget = handle_stack_sidebar(ps, attribute_names, attribute_values);
    }
    else if (g_strcmp0(element_name, "button") == 0) {
        widget = handle_button(ps, attribute_names, attribute_values);
    }
    else if (g_strcmp0(element_name, "entry") == 0) {
        widget = handle_entry(ps, attribute_names, attribute_values);
    }
    else if (g_strcmp0(element_name, "spin-button") == 0) {
        widget = handle_spin_button(ps, attribute_names, attribute_values);
    }
    else if (g_strcmp0(element_name, "dropdown") == 0) {
        widget = handle_dropdown(ps, attribute_names, attribute_values);
    }
    else if (g_strcmp0(element_name, "calendar") == 0) {
        widget = handle_calendar(ps, attribute_names, attribute_values);
    }
    else if (g_strcmp0(element_name, "label") == 0) {
        widget = handle_label(ps, attribute_names, attribute_values);
    }
    else if (g_strcmp0(element_name, "image") == 0) {
        widget = handle_image(ps, attribute_names, attribute_values);
    }
    else if (g_strcmp0(element_name, "progress-bar") == 0) {
        widget = handle_progress_bar(ps, attribute_names, attribute_values);
    }
    else if (g_strcmp0(element_name, "spinner") == 0) {
        widget = handle_spinner(ps, attribute_names, attribute_values);
    }
    else if (g_strcmp0(element_name, "separator") == 0) {
        widget = handle_separator(ps, attribute_names, attribute_values);
    }
    else if (g_strcmp0(element_name, "checkbox") == 0) {
        widget = handle_checkbox(ps, attribute_names, attribute_values);
    }
    else if (g_strcmp0(element_name, "switch") == 0) {
        widget = handle_switch(ps, attribute_names, attribute_values);
    }
    else if (g_strcmp0(element_name, "radio-button") == 0) {
        widget = handle_radio_button(ps, attribute_names, attribute_values);
    }
    else if (g_strcmp0(element_name, "stack-page") == 0) {
        is_container = true; /* stack page contains a box with the elements */
        
        GtkWidget *page_wrapper = create_box(&(box_config){
            .orientation = GTK_ORIENTATION_VERTICAL,
            .style = parse_style(attribute_names, attribute_values),
        });
        
        const char *name  = find_attr(attribute_names, attribute_values, "name");
        const char *title = find_attr(attribute_names, attribute_values, "title");
        GtkWidget *parent_stack = current_parent(ps);
        if (parent_stack) {
            stack_add_page(parent_stack, page_wrapper, name ? name : "page", title ? title : "Page");
        }
        
        widget = page_wrapper;
    }
    else {
        g_warning("xml_parser: unknown element '%s'", element_name);
        return;
    }

    if (!widget) return;

    /* Register id if present */
    const char *id = find_attr(attribute_names, attribute_values, "id");
    if (id) register_id(id, widget);

    /* Add to parent, or mark as root */
    if (ps->depth == 0) {
        ps->result = widget;
    } else {
        if (g_strcmp0(element_name, "stack-page") != 0) {
            add_to_parent(ps, widget, attribute_names, attribute_values);
        }
    }

    /* Container elements get pushed so children go inside them */
    if (is_container) {
        push(ps, widget);
    }
}

static void xml_end_element(GMarkupParseContext *context,
                            const gchar         *element_name,
                            gpointer             user_data,
                            GError             **error) {
    (void)context;
    (void)error;
    parser_state *ps = user_data;

    if (g_strcmp0(element_name, "menu-item") == 0) {
        if (ps->menu_state && ps->menu_state->item_stack->len > 0) {
            g_ptr_array_set_size(ps->menu_state->item_stack,
                                 ps->menu_state->item_stack->len - 1);
        }
        return;
    }

    if (g_strcmp0(element_name, "menu-section") == 0) {
        if (ps->menu_state) {
            ps->menu_state->current_section = NULL;
        }
        return;
    }

    if (g_strcmp0(element_name, "menubar") == 0) {
        if (!ps->menu_state) {
            return;
        }
        GtkWidget *menubar = build_menubar_widget(ps, ps->menu_state);
        if (menubar) {
            if (ps->depth == 0) {
                ps->result = menubar;
            } else {
                add_to_parent_at(ps, menubar,
                                 ps->menu_state->layout_col,
                                 ps->menu_state->layout_row,
                                 ps->menu_state->layout_colspan,
                                 ps->menu_state->layout_rowspan);
            }
        }
        menubar_state_free(ps->menu_state);
        ps->menu_state = NULL;
        return;
    }

    /* Pop from stack when closing a container element */
    if (g_strcmp0(element_name, "window") == 0 ||
        g_strcmp0(element_name, "box") == 0 ||
        g_strcmp0(element_name, "grid") == 0 ||
        g_strcmp0(element_name, "stack") == 0 ||
        g_strcmp0(element_name, "stack-page") == 0) {
        if (ps->depth > 0)
            ps->depth--;
    }
}

/* ── Public API ── */

GtkWidget *xml_parse_file(const char *filename, const xml_parse_context *ctx) {
    g_return_val_if_fail(filename != NULL, NULL);
    g_return_val_if_fail(ctx != NULL, NULL);
    g_return_val_if_fail(ctx->app != NULL, NULL);

    /* Reset ID table */
    for (int i = 0; i < g_id_count; i++)
        g_free((char *)g_id_table[i].id);
    g_id_count = 0;

    /* Read file */
    gchar *content = NULL;
    gsize length = 0;
    GError *err = NULL;
    if (!g_file_get_contents(filename, &content, &length, &err)) {
        g_warning("xml_parser: failed to read '%s': %s", filename, err->message);
        g_error_free(err);
        return NULL;
    }

    /* Set up parser */
    parser_state ps = {
        .ctx   = ctx,
        .depth = 0,
        .result = NULL,
        .string_arrays = g_ptr_array_new(),
        .menu_state = NULL,
    };
    memset(ps.stack, 0, sizeof(ps.stack));

    GMarkupParser parser = {
        .start_element = xml_start_element,
        .end_element   = xml_end_element,
        .text          = NULL,
        .passthrough   = NULL,
        .error         = NULL,
    };

    GMarkupParseContext *parse_ctx =
        g_markup_parse_context_new(&parser, 0, &ps, NULL);

    if (!g_markup_parse_context_parse(parse_ctx, content, (gssize)length, &err)) {
        g_warning("xml_parser: parse error: %s", err->message);
        g_error_free(err);
    }

    g_markup_parse_context_end_parse(parse_ctx, NULL);
    g_markup_parse_context_free(parse_ctx);
    g_free(content);
    
    // Cleanup string arrays created during parse
    for (guint i = 0; i < ps.string_arrays->len; i++) {
        g_strfreev(g_ptr_array_index(ps.string_arrays, i));
    }
    g_ptr_array_free(ps.string_arrays, TRUE);

    return ps.result;
}
