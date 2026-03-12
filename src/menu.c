#include "menu.h"

static guint action_counter = 0;

static char *create_action_name(void) {
    action_counter++;
    return g_strdup_printf("menu_item_%u", action_counter);
}

static void register_item_action(const menubar_config *config,
                                 const menu_item_config *item,
                                 const char *action_name) {
    if (config == NULL || config->action_map == NULL || item == NULL || item->on_activate == NULL || action_name == NULL) {
        return;
    }

    GSimpleAction *action = g_simple_action_new(action_name, NULL);
    g_signal_connect(action, "activate", G_CALLBACK(item->on_activate), item->user_data);
    g_action_map_add_action(config->action_map, G_ACTION(action));
    g_object_unref(action);
}

static void append_item_recursive(GMenu *menu,
                                  const menu_item_config *item,
                                  const menubar_config *config) {
    if (item == NULL || item->enabled == false) {
        return;
    }

    if (item->kind == MENU_ITEM_SUBMENU && item->children != NULL && item->child_count > 0) {
        GMenu *submenu = g_menu_new();
        for (size_t i = 0; i < item->child_count; i++) {
            append_item_recursive(submenu, &item->children[i], config);
        }
        g_menu_append_submenu(menu, item->label ? item->label : "", G_MENU_MODEL(submenu));
        g_object_unref(submenu);
        return;
    }

    if (item->kind == MENU_ITEM_SECTION && item->children != NULL && item->child_count > 0) {
        GMenu *section = g_menu_new();
        for (size_t i = 0; i < item->child_count; i++) {
            append_item_recursive(section, &item->children[i], config);
        }
        g_menu_append_section(menu, item->label, G_MENU_MODEL(section));
        g_object_unref(section);
        return;
    }

    char *action_name = create_action_name();
    register_item_action(config, item, action_name);

    char *detailed_action = g_strdup_printf("app.%s", action_name);
    g_menu_append(menu, item->label ? item->label : "", detailed_action);

    g_free(detailed_action);
    g_free(action_name);
}

static GMenuModel *build_menu_model(const menubar_config *config) {
    GMenu *root = g_menu_new();

    if (config == NULL) {
        return G_MENU_MODEL(root);
    }

    for (size_t i = 0; i < config->section_count; i++) {
        const menu_section_config *section = &config->sections[i];
        GMenu *submenu = g_menu_new();

        for (size_t j = 0; j < section->item_count; j++) {
            append_item_recursive(submenu, &section->items[j], config);
        }

        g_menu_append_submenu(root, section->label ? section->label : "", G_MENU_MODEL(submenu));
        g_object_unref(submenu);
    }

    return G_MENU_MODEL(root);
}

GtkWidget *create_menubar(const menubar_config *config) {
    GMenuModel *model = build_menu_model(config);
    GtkWidget *bar = gtk_popover_menu_bar_new_from_model(model);
    if (config != NULL) {
        apply_widget_style(bar, &config->style);
    }
    g_object_unref(model);
    return bar;
}

GtkWidget *create_menu_panel(const menubar_config *config) {
    GMenuModel *model = build_menu_model(config);
    GtkWidget *panel = gtk_popover_menu_new_from_model(model);

    if (config != NULL) {
        gtk_orientable_set_orientation(
            GTK_ORIENTABLE(panel),
            config->layout == MENU_LAYOUT_VERTICAL ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL
        );
        gtk_popover_set_has_arrow(GTK_POPOVER(panel), config->show_arrow_indicators);
        apply_widget_style(panel, &config->style);
    }

    g_object_unref(model);
    return panel;
}
