#ifndef GTK_WRAPPER_INPUT_H
#define GTK_WRAPPER_INPUT_H

#include <gtk/gtk.h>
#include <stdbool.h>

#include "common.h"

/* Entry Shortcuts */
#define CREATE_PASSWORD_ENTRY(ph) create_entry(&(entry_config){.placeholder = ph, .password = true, .purpose_hint = "password"})
#define CREATE_NUMERIC_ENTRY(ph)  create_entry(&(entry_config){.placeholder = ph, .purpose_hint = "number"})
#define CREATE_EMAIL_ENTRY(ph)    create_entry(&(entry_config){.placeholder = ph, .purpose_hint = "email"})
#define CREATE_URL_ENTRY(ph)      create_entry(&(entry_config){.placeholder = ph, .purpose_hint = "url"})

/* Spin Button Presets */
#define SPIN_PERCENT(v) (spin_button_config){.min_value = 0, .max_value = 100, .step = 1, .value = v, .digits = 0}
#define SPIN_AGE(v)     (spin_button_config){.min_value = 0, .max_value = 120, .step = 1, .value = v, .digits = 0}
#define SPIN_YEAR(v)    (spin_button_config){.min_value = 1900, .max_value = 2100, .step = 1, .value = v, .digits = 0}

typedef struct {
    const char *placeholder;
    const char *default_text;
    const char *purpose_hint;
    bool password;
    bool editable;
    int max_length;
    widget_style_config style;
    void (*on_changed)(GtkEditable *editable, gpointer user_data);
    gpointer user_data;
} entry_config;

typedef struct {
    const char **options;
    guint selected_index;
    bool enable_search;
    widget_style_config style;
    void (*on_selected)(GtkDropDown *dropdown, GParamSpec *pspec, gpointer user_data);
    gpointer user_data;
} dropdown_config;

typedef struct {
    double min_value;
    double max_value;
    double step;
    double page;
    double value;
    guint digits;
    bool numeric_only;
    bool wrap;
    widget_style_config style;
    void (*on_value_changed)(GtkSpinButton *spin, gpointer user_data);
    gpointer user_data;
} spin_button_config;

typedef struct {
    const char *default_text;
    bool editable;
    bool monospace;
    bool wrap;
    widget_style_config style;
} text_view_config;

GtkWidget *create_entry(const entry_config *config);
GtkWidget *create_dropdown(const dropdown_config *config);
GtkWidget *create_spin_button(const spin_button_config *config);
GtkWidget *create_text_view(const text_view_config *config);

#endif
