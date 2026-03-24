#include "widgets/theme.h"

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
        "}"
        ".vscode-window {"
        "  background: #1e1e1e;"
        "  color: #d4d4d4;"
        "}"
        ".vscode-headerbar {"
        "  background: #2d2d30;"
        "  color: #d4d4d4;"
        "}"
        ".vscode-menubar {"
        "  background: #252526;"
        "  color: #cccccc;"
        "}"
        ".vscode-main {"
        "  background: #1e1e1e;"
        "}"
        ".vscode-sidebar {"
        "  background: #252526;"
        "  color: #cccccc;"
        "  padding: 6px;"
        "}"
        ".vscode-editor-shell {"
        "  background: #1e1e1e;"
        "}"
        ".vscode-tabstrip {"
        "  background: #2d2d30;"
        "}"
        ".vscode-tab-title {"
        "  color: #ffffff;"
        "  font-weight: 700;"
        "}"
        ".vscode-editor {"
        "  background: #1e1e1e;"
        "  border: 1px solid #3e3e42;"
        "  border-radius: 4px;"
        "}"
        ".vscode-editor textview {"
        "  background: #1e1e1e;"
        "  color: #d4d4d4;"
        "  caret-color: #d4d4d4;"
        "}"
        ".vscode-statusbar {"
        "  background: #007acc;"
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
