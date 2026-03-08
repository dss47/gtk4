#ifndef GTK_WRAPPER_DISPLAY_H
#define GTK_WRAPPER_DISPLAY_H

#include <gtk/gtk.h>
#include <stdbool.h>

#include "common.h"

/* Label Shortcuts */
#define CREATE_TITLE(t)    create_label(&(label_config){.text = t, .style = {.css_class = "title-1", .margin_bottom = 12}})
#define CREATE_SUBTITLE(t) create_label(&(label_config){.text = t, .style = {.css_class = "title-2", .margin_bottom = 8}})

/* Progress Bar Presets */
#define PROGRESS_START 0.0
#define PROGRESS_HALF  0.5
#define PROGRESS_FULL  1.0

typedef struct {
    double fraction;
    const char *text;
    GtkOrientation orientation;
    bool show_text;
    bool inverted;
    bool pulse_once;
    widget_style_config style;
} progress_bar_config;

typedef struct {
    int size;
    bool spinning;
    widget_style_config style;
} spinner_config;

typedef struct {
    const char *text;
    bool selectable;
    bool wrap;
    PangoEllipsizeMode ellipsize;
    int max_width_chars;
    widget_style_config style;
} label_config;

typedef struct {
    const char *icon_name;
    const char *file_path;
    int pixel_size;
    bool keep_aspect;
    widget_style_config style;
} image_config;

GtkWidget *create_progress_bar(const progress_bar_config *config);
GtkWidget *create_spinner(const spinner_config *config);
GtkWidget *create_label(const label_config *config);
GtkWidget *create_image(const image_config *config);

#endif
