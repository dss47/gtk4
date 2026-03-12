#ifndef GTK_WRAPPER_COMMON_H
#define GTK_WRAPPER_COMMON_H

#include <gtk/gtk.h>
#include <stdbool.h>

/* Alignment Aliases */
#define ALIGN_START  GTK_ALIGN_START
#define ALIGN_END    GTK_ALIGN_END
#define ALIGN_CENTER GTK_ALIGN_CENTER
#define ALIGN_FILL   GTK_ALIGN_FILL

/* Standard Margins */
#define MARGIN_NONE   0
#define MARGIN_TINY   4
#define MARGIN_SMALL  8
#define MARGIN_MEDIUM 16
#define MARGIN_LARGE  24

typedef struct {
    const char *css_class;
    const char **css_classes;
    int width_request;
    int height_request;
    int margin_top;
    int margin_bottom;
    int margin_start;
    int margin_end;
    GtkAlign halign;
    GtkAlign valign;
    bool set_halign;
    bool set_valign;
    bool set_hexpand;
    bool set_vexpand;
    bool set_sensitive;
    bool set_visible;
    bool hexpand;
    bool vexpand;
    bool sensitive;
    bool visible;
    const char *tooltip;
} widget_style_config;

void apply_widget_style(GtkWidget *widget, const widget_style_config *style);

#endif
