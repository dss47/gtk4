#ifndef GTK_WRAPPER_DEMO_APP_H
#define GTK_WRAPPER_DEMO_APP_H

#include <gtk/gtk.h>

typedef struct {
  GtkWidget *status_label;
  GtkWidget *progress;
  GtkWidget *spinner;
  GtkWidget *switch_widget;
  GtkWidget *icon_size_label;
  GtkWidget *icon_preview;
  int icon_counter;
  bool spinner_running;
} showcase_state;

void demo_app_start(GtkApplication *app, gpointer user_data);

#endif
