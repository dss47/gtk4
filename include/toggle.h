#ifndef GTK_WRAPPER_TOGGLE_H
#define GTK_WRAPPER_TOGGLE_H

#include <gtk/gtk.h>
#include <stdbool.h>

#include "common.h"

typedef struct {
    const char *label;
    bool active;
    bool inconsistent;
    widget_style_config style;
    void (*on_toggled)(GtkCheckButton *button, gpointer user_data);
    gpointer user_data;
} checkbox_config;

typedef struct {
    const char *label;
    bool active;
    bool state;
    widget_style_config style;
    void (*on_active_changed)(GtkSwitch *sw, GParamSpec *pspec, gpointer user_data);
    gpointer user_data;
} switch_config;

GtkWidget *create_checkbox(const checkbox_config *config);
GtkWidget *create_switch_row(const switch_config *config, GtkWidget **out_switch);

#endif
