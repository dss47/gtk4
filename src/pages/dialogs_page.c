#include "pages.h"
#include "widgets/container.h"
#include "widgets/button.h"
#include "widgets/dialog.h"
#include "widgets/date.h"

extern void on_show_dialog(GtkButton *button, gpointer user_data);
extern void on_day_selected(GtkCalendar *calendar, gpointer user_data);

GtkWidget *make_dialogs_page(showcase_state *state) {
  GDateTime *now = g_date_time_new_now_local();
  GtkWidget *box = create_box(&(box_config){
      .orientation = GTK_ORIENTATION_VERTICAL,
      .spacing = 10,
      .homogeneous = false,
      .style = {.margin_top = 16,
                .margin_bottom = 16,
                .margin_start = 16,
                .margin_end = 16},
  });

  container_add(box, create_button(&(button_config){
                                   .label = "Open Alert",
                                   .icon_name = "dialog-information-symbolic",
                                   .theme_variant = "primary",
                                   .use_underline = false,
                                   .has_frame = true,
                                   .can_shrink = true,
                                   .icon_size = 16,
                                   .style = {.width_request = 180},
                                   .on_clicked = on_show_dialog,
                                   .user_data = state,
                               }), 0, 0, 0, 0);

  container_add(box, create_calendar(&(calendar_config){
                                   .selected_date = now,
                                   .show_day_names = true,
                                   .show_week_numbers = true,
                                   .no_month_change = false,
                                   .style = {.margin_top = 8},
                                   .on_day_selected = on_day_selected,
                                   .user_data = state,
                               }), 0, 0, 0, 0);
  g_date_time_unref(now);
  return box;
}
