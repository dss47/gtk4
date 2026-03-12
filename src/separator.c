#include "separator.h"

GtkWidget *create_separator(const separator_config *config) {
    GtkWidget *separator = gtk_separator_new(config ? config->orientation : GTK_ORIENTATION_HORIZONTAL);
    if (config != NULL) {
        apply_widget_style(separator, &config->style);
    }
    return separator;
}
