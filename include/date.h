#ifndef GTK_WRAPPER_DATE_H
#define GTK_WRAPPER_DATE_H

#include <gtk/gtk.h>
#include <stdbool.h>

#include "common.h"

typedef struct {
    GDateTime *selected_date;
    bool show_day_names;
    bool show_week_numbers;
    bool no_month_change;
    widget_style_config style;
    void (*on_day_selected)(GtkCalendar *calendar, gpointer user_data);
    gpointer user_data;
} calendar_config;

GtkWidget *create_calendar(const calendar_config *config);

#endif
