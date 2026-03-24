#ifndef GTK_WRAPPER_WINDOW_H
#define GTK_WRAPPER_WINDOW_H

#include <gtk/gtk.h>
#include <stdbool.h>

#include "common.h"

/* Standard Window Sizes */
#define WIN_SIZE_SMALL 400, 300
#define WIN_SIZE_MEDIUM 800, 600
#define WIN_SIZE_LARGE 1200, 900

/* Window Creation Shortcut */
#define CREATE_SIMPLE_WINDOW(app, t)                                           \
  create_window(app, &(window_config){.title = t,                              \
                                      .default_width = 800,                    \
                                      .default_height = 600,                   \
                                      .resizable = true})

typedef struct {
  const char *title;
  const char *headerbar_title;
  const char *headerbar_subtitle;
  const char *headerbar_background_color;
  int headerbar_height;
  const char *icon_name;
  const char *widget_name;
  const char *background_color;
  const char *background_image_path;
  int default_width;
  int default_height;
  int min_width;
  int min_height;
  int max_width;
  int max_height;
  bool resizable;
  bool decorated;
  bool modal;
  bool use_headerbar;
  bool headerbar_show_title_buttons;
  bool headerbar_show_close_button;
  bool headerbar_show_minimize_button;
  bool headerbar_show_maximize_button;
  bool maximized;
  bool present_on_create;
  widget_style_config style;
} window_config;

GtkWidget *create_window(GtkApplication *app, const window_config *config);

#endif
