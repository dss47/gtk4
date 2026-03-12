#include "window.h"

static void apply_window_background(const window_config *config, GtkWidget *window) {
    if (config == NULL || window == NULL || config->background_color == NULL || config->background_color[0] == '\0') {
        return;
    }

    const char *name = config->widget_name;
    if (name == NULL || name[0] == '\0') {
        name = "wrapper-window";
        gtk_widget_set_name(window, name);
    }

    GtkCssProvider *provider = gtk_css_provider_new();
    char *css;

    if (config->background_image_path != NULL && config->background_image_path[0] != '\0') {
        css = g_strdup_printf(
            "window#%s, window#%s > * {"
            "background-color: %s;"
            "background-image: url('%s');"
            "background-size: cover;"
            "background-position: center;"
            "}",
            name,
            name,
            config->background_color,
            config->background_image_path
        );
    } else {
        css = g_strdup_printf(
            "window#%s, window#%s > * { background-color: %s; }",
            name,
            name,
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
    gtk_window_set_decorated(GTK_WINDOW(window), config->decorated);
    gtk_window_set_modal(GTK_WINDOW(window), config->modal);

    if (config->min_width > 0 || config->min_height > 0) {
        gtk_widget_set_size_request(window, config->min_width, config->min_height);
    }

    apply_window_background(config, window);

    apply_widget_style(window, &config->style);

    if (config->maximized) {
        gtk_window_maximize(GTK_WINDOW(window));
    }
    if (config->present_on_create) {
        gtk_window_present(GTK_WINDOW(window));
    }

    return window;
}
