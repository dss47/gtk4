#include "theme.h"

void apply_demo_theme(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    const char *css =
        ".btn-primary {"
        "  background: linear-gradient(180deg, #1f8c7a, #176b5e);"
        "  color: #ffffff;"
        "}"
        ".btn-success {"
        "  background: linear-gradient(180deg, #3c8f28, #2f6f1f);"
        "  color: #ffffff;"
        "}"
        ".btn-danger {"
        "  background: linear-gradient(180deg, #b74040, #8e2f2f);"
        "  color: #ffffff;"
        "}";

    gtk_css_provider_load_from_string(provider, css);
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(provider);
}
