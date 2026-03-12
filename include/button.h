#ifndef GTK_WRAPPER_BUTTON_H
#define GTK_WRAPPER_BUTTON_H

#include <gtk/gtk.h>
#include <stdbool.h>
#include "common.h"

/* Button Theme Variants */
#define BTN_PRIMARY "primary"
#define BTN_SUCCESS "success"
#define BTN_DANGER  "danger"
#define BTN_WARNING "warning"

typedef struct {
    const char *label;
    const char *icon_name;
    const char *theme_variant;
    bool use_underline;
    bool has_frame;
    bool can_shrink;
    int icon_size;
    widget_style_config style;
    void (*on_clicked)(GtkButton *button, gpointer user_data);
    gpointer user_data;
} button_config;

typedef struct {
    const char *label;
    GtkWidget *group_with;
    bool is_active;
    widget_style_config style;
    void (*on_toggled)(GtkCheckButton *button, gpointer user_data);
    gpointer user_data;
} radio_button_config;

GtkWidget *create_button(const button_config *config);
GtkWidget *create_radio_button(const radio_button_config *config);

#endif
