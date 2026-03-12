#ifndef GTK_WRAPPER_SEPARATOR_H
#define GTK_WRAPPER_SEPARATOR_H

#include <gtk/gtk.h>

#include "common.h"

typedef struct {
    GtkOrientation orientation;
    widget_style_config style;
} separator_config;

GtkWidget *create_separator(const separator_config *config);

#endif
