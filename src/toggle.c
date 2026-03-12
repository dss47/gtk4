#include "toggle.h"

GtkWidget *create_checkbox(const checkbox_config *config) {
    GtkWidget *checkbox = gtk_check_button_new_with_label(config && config->label ? config->label : "");

    if (config == NULL) {
        return checkbox;
    }

    gtk_check_button_set_active(GTK_CHECK_BUTTON(checkbox), config->active);
    gtk_check_button_set_inconsistent(GTK_CHECK_BUTTON(checkbox), config->inconsistent);
    apply_widget_style(checkbox, &config->style);

    if (config->on_toggled != NULL) {
        g_signal_connect(checkbox, "toggled", G_CALLBACK(config->on_toggled), config->user_data);
    }

    return checkbox;
}

GtkWidget *create_switch_row(const switch_config *config, GtkWidget **out_switch) {
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *label = gtk_label_new(config && config->label ? config->label : "");
    GtkWidget *sw = gtk_switch_new();

    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_halign(sw, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(row), label);
    gtk_box_append(GTK_BOX(row), sw);

    if (config != NULL) {
        gtk_switch_set_active(GTK_SWITCH(sw), config->active);
        gtk_switch_set_state(GTK_SWITCH(sw), config->state);
        apply_widget_style(row, &config->style);

        if (config->on_active_changed != NULL) {
            g_signal_connect(sw, "notify::active", G_CALLBACK(config->on_active_changed), config->user_data);
        }
    }

    if (out_switch != NULL) {
        *out_switch = sw;
    }

    return row;
}
