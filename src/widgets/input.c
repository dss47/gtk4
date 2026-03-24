#include "widgets/input.h"

#include <string.h>

static GtkInputPurpose parse_input_purpose(const char *hint) {
    if (hint == NULL) {
        return GTK_INPUT_PURPOSE_FREE_FORM;
    }
    if (strcmp(hint, "email") == 0) {
        return GTK_INPUT_PURPOSE_EMAIL;
    }
    if (strcmp(hint, "number") == 0) {
        return GTK_INPUT_PURPOSE_NUMBER;
    }
    if (strcmp(hint, "phone") == 0) {
        return GTK_INPUT_PURPOSE_PHONE;
    }
    if (strcmp(hint, "url") == 0) {
        return GTK_INPUT_PURPOSE_URL;
    }
    return GTK_INPUT_PURPOSE_FREE_FORM;
}

GtkWidget *create_entry(const entry_config *config) {
    GtkWidget *entry = gtk_entry_new();

    if (config == NULL) {
        return entry;
    }

    if (config->placeholder) {
        gtk_entry_set_placeholder_text(GTK_ENTRY(entry), config->placeholder);
    }
    if (config->default_text) {
        gtk_editable_set_text(GTK_EDITABLE(entry), config->default_text);
    }
    gtk_entry_set_input_purpose(GTK_ENTRY(entry), parse_input_purpose(config->purpose_hint));
    gtk_entry_set_visibility(GTK_ENTRY(entry), !config->password);
    gtk_editable_set_editable(GTK_EDITABLE(entry), config->editable);
    if (config->max_length > 0) {
        gtk_entry_set_max_length(GTK_ENTRY(entry), config->max_length);
    }
    apply_widget_style(entry, &config->style);

    if (config->on_changed) {
        g_signal_connect(entry, "changed", G_CALLBACK(config->on_changed), config->user_data);
    }

    return entry;
}

GtkWidget *create_dropdown(const dropdown_config *config) {
    GtkWidget *dropdown;

    if (config == NULL || config->options == NULL) {
        return gtk_drop_down_new(NULL, NULL);
    }

    dropdown = gtk_drop_down_new_from_strings(config->options);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(dropdown), config->selected_index);
    gtk_drop_down_set_enable_search(GTK_DROP_DOWN(dropdown), config->enable_search);
    apply_widget_style(dropdown, &config->style);

    if (config->on_selected) {
        g_signal_connect(dropdown, "notify::selected", G_CALLBACK(config->on_selected), config->user_data);
    }

    return dropdown;
}

GtkWidget *create_spin_button(const spin_button_config *config) {
    GtkAdjustment *adj;
    GtkWidget *spin;

    if (config == NULL) {
        adj = gtk_adjustment_new(0.0, 0.0, 100.0, 1.0, 5.0, 0.0);
        return gtk_spin_button_new(adj, 1.0, 0);
    }

    adj = gtk_adjustment_new(config->value, config->min_value, config->max_value, config->step, config->page, 0.0);
    spin = gtk_spin_button_new(adj, config->step, config->digits);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spin), config->numeric_only);
    gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spin), config->wrap);
    apply_widget_style(spin, &config->style);

    if (config->on_value_changed) {
        g_signal_connect(spin, "value-changed", G_CALLBACK(config->on_value_changed), config->user_data);
    }

    return spin;
}

GtkWidget *create_text_view(const text_view_config *config) {
    GtkWidget *scroll = gtk_scrolled_window_new();
    GtkWidget *view = gtk_text_view_new();

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);

    if (config != NULL) {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
        if (config->default_text != NULL) {
            gtk_text_buffer_set_text(buffer, config->default_text, -1);
        }

        gtk_text_view_set_editable(GTK_TEXT_VIEW(view), config->editable);
        gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), config->editable);
        gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), config->monospace);
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view),
            config->wrap ? GTK_WRAP_WORD_CHAR : GTK_WRAP_NONE);

        apply_widget_style(scroll, &config->style);
    }

    return scroll;
}
