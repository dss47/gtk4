#include "widgets/window.h"

#include <string.h>

static bool is_hex_digit(char c) {
    return (c >= '0' && c <= '9') ||
           (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}

static char *normalize_css_color(const char *color) {
    if (color == NULL || color[0] == '\0') {
        return NULL;
    }

    size_t len = strlen(color);
    if (color[0] == '#') {
        return g_strdup(color);
    }

    bool all_hex = (len == 3 || len == 6 || len == 8);
    if (all_hex) {
        for (size_t i = 0; i < len; i++) {
            if (!is_hex_digit(color[i])) {
                all_hex = false;
                break;
            }
        }
    }

    if (all_hex) {
        return g_strdup_printf("#%s", color);
    }

    return NULL;
}

static void apply_headerbar_background(GtkWidget *headerbar,
                                       const char *background_color) {
    if (headerbar == NULL || !GTK_IS_HEADER_BAR(headerbar)) {
        return;
    }

    char *name = g_strdup_printf("wrapper-headerbar-%p", (void *)headerbar);
    gtk_widget_set_name(headerbar, name);

    GtkCssProvider *provider = gtk_css_provider_new();
    char *css;
    char *normalized = normalize_css_color(background_color);
    if (normalized != NULL) {
        css = g_strdup_printf(
            "headerbar#%s { background-image: none; background-color: %s; }",
            name,
            normalized
        );
    } else {
        css = g_strdup_printf(
            "headerbar#%s { background-image: none; }",
            name
        );
    }

    gtk_css_provider_load_from_string(provider, css);
    gtk_style_context_add_provider_for_display(
        gtk_widget_get_display(headerbar),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    g_free(css);
    g_free(normalized);
    g_object_unref(provider);
    g_free(name);
}

static void apply_headerbar_height_css(GtkWidget *headerbar, int height) {
    if (headerbar == NULL || !GTK_IS_HEADER_BAR(headerbar) || height <= 0) {
        return;
    }

    const char *name = gtk_widget_get_name(headerbar);
    if (name == NULL || name[0] == '\0') {
        return;
    }

    GtkCssProvider *provider = gtk_css_provider_new();
    char *css = g_strdup_printf(
        "headerbar#%s {"
        " min-height: %dpx;"
        " padding-top: 0;"
        " padding-bottom: 0;"
        "}"
        "headerbar#%s > * {"
        " min-height: %dpx;"
        " margin-top: 0;"
        " margin-bottom: 0;"
        " padding-top: 0;"
        " padding-bottom: 0;"
        "}",
        name, height,
        name, height
    );

    gtk_css_provider_load_from_string(provider, css);
    gtk_style_context_add_provider_for_display(
        gtk_widget_get_display(headerbar),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    g_free(css);
    g_object_unref(provider);
}

static char *build_headerbar_decoration_layout(const window_config *config) {
    GString *controls = g_string_new("");

    if (config->headerbar_show_minimize_button) {
        g_string_append(controls, "minimize");
    }
    if (config->headerbar_show_maximize_button) {
        if (controls->len > 0) {
            g_string_append_c(controls, ',');
        }
        g_string_append(controls, "maximize");
    }
    if (config->headerbar_show_close_button) {
        if (controls->len > 0) {
            g_string_append_c(controls, ',');
        }
        g_string_append(controls, "close");
    }

    char *layout = g_strdup_printf(":%s", controls->str);
    g_string_free(controls, TRUE);
    return layout;
}

static void apply_window_headerbar(const window_config *config, GtkWidget *window) {
    if (config == NULL || window == NULL || !config->use_headerbar) {
        return;
    }

    GtkWidget *headerbar = gtk_header_bar_new();
    if (config->headerbar_height > 0) {
        gtk_widget_set_size_request(headerbar, -1, config->headerbar_height);
    }

    bool tiny_headerbar = (config->headerbar_height > 0 && config->headerbar_height < 18);
    const char *title = config->headerbar_title ? config->headerbar_title : config->title;
    const char *subtitle = config->headerbar_subtitle;
    if (!tiny_headerbar && title != NULL && title[0] != '\0') {
        GtkWidget *title_label = gtk_label_new(title);
        gtk_widget_add_css_class(title_label, "title");
        gtk_widget_set_valign(title_label, GTK_ALIGN_CENTER);
        gtk_header_bar_set_title_widget(GTK_HEADER_BAR(headerbar), title_label);
    }

    if (!tiny_headerbar && subtitle != NULL && subtitle[0] != '\0') {
        GtkWidget *subtitle_label = gtk_label_new(subtitle);
        gtk_widget_add_css_class(subtitle_label, "dim-label");
        gtk_widget_set_valign(subtitle_label, GTK_ALIGN_CENTER);
        gtk_header_bar_pack_end(GTK_HEADER_BAR(headerbar), subtitle_label);
    }

    gtk_header_bar_set_show_title_buttons(
        GTK_HEADER_BAR(headerbar),
        tiny_headerbar ? false : config->headerbar_show_title_buttons
    );

    char *decoration_layout = build_headerbar_decoration_layout(config);
    gtk_header_bar_set_decoration_layout(GTK_HEADER_BAR(headerbar), decoration_layout);
    g_free(decoration_layout);

    apply_headerbar_background(headerbar, config->headerbar_background_color);
    apply_headerbar_height_css(headerbar, config->headerbar_height);

    gtk_window_set_titlebar(GTK_WINDOW(window), headerbar);
}

static void apply_window_background(const window_config *config, GtkWidget *window) {
    if (config == NULL || window == NULL) {
        return;
    }

    bool has_color = config->background_color != NULL && config->background_color[0] != '\0';
    bool has_image = config->background_image_path != NULL && config->background_image_path[0] != '\0';
    if (!has_color && !has_image) {
        return;
    }

    const char *name = config->widget_name;
    if (name == NULL || name[0] == '\0') {
        name = "wrapper-window";
        gtk_widget_set_name(window, name);
    }

    GtkCssProvider *provider = gtk_css_provider_new();
    char *css;

    if (has_image && has_color) {
        char *path = g_canonicalize_filename(config->background_image_path, NULL);
        css = g_strdup_printf(
            "window#%s, window#%s > * {"
            "background-color: %s;"
            "background-image: url('file://%s');"
            "background-size: cover;"
            "background-position: center;"
            "}",
            name, name,
            config->background_color,
            path
        );
        g_free(path);
    } else if (has_image) {
        char *path = g_canonicalize_filename(config->background_image_path, NULL);
        css = g_strdup_printf(
            "window#%s, window#%s > * {"
            "background-image: url('file://%s');"
            "background-size: cover;"
            "background-position: center;"
            "}",
            name, name,
            path
        );
        g_free(path);
    } else {
        css = g_strdup_printf(
            "window#%s, window#%s > * { background-color: %s; }",
            name, name,
            config->background_color
        );
    }

    gtk_css_provider_load_from_string(provider, css);
    gtk_style_context_add_provider_for_display(
        gtk_widget_get_display(window),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    g_free(css);
    g_object_unref(provider);
}

GtkWidget *create_window(GtkApplication *app, const window_config *config) {
    GtkWidget *window = gtk_application_window_new(app);
    if (config == NULL) {
        return window;
    }

    if (config->title) {
        gtk_window_set_title(GTK_WINDOW(window), config->title);
    }
    if (config->icon_name) {
        gtk_window_set_icon_name(GTK_WINDOW(window), config->icon_name);
    }
    if (config->widget_name) {
        gtk_widget_set_name(window, config->widget_name);
    }

    gtk_window_set_default_size(GTK_WINDOW(window), config->default_width, config->default_height);
    gtk_window_set_resizable(GTK_WINDOW(window), config->resizable);

    g_object_set_data(
        G_OBJECT(window),
        "wrapper-use-headerbar",
        GINT_TO_POINTER(config->use_headerbar ? 1 : 0)
    );

    /* Headerbar disabled means fully borderless: hide both custom and system title bars. */
    gtk_window_set_decorated(GTK_WINDOW(window),
                             config->use_headerbar ? true : false);
    gtk_window_set_modal(GTK_WINDOW(window), config->modal);

    if (config->min_width > 0 || config->min_height > 0) {
        gtk_widget_set_size_request(window, config->min_width, config->min_height);
    }

    apply_window_background(config, window);
    if (config->use_headerbar) {
        apply_window_headerbar(config, window);
    } else {
        gtk_window_set_titlebar(GTK_WINDOW(window), NULL);
    }

    apply_widget_style(window, &config->style);

    if (config->maximized) {
        gtk_window_maximize(GTK_WINDOW(window));
    }
    if (config->present_on_create) {
        gtk_window_present(GTK_WINDOW(window));
    }

    return window;
}
