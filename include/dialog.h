#ifndef GTK_WRAPPER_DIALOG_H
#define GTK_WRAPPER_DIALOG_H

#include <gtk/gtk.h>
#include <stdbool.h>
#include <stddef.h>
#include "common.h"

typedef struct {
    const char *label;
    const char *appearance;
} alert_button_config;

typedef struct {
    GtkWindow *parent;
    const char *message;
    const char *detail;
    const alert_button_config *buttons;
    size_t button_count;
    int default_button;
    int cancel_button;
    bool modal;
    void (*on_response)(int response_index, gpointer user_data);
    gpointer user_data;
} alert_dialog_config;

void show_alert_dialog(const alert_dialog_config *config);

#endif
