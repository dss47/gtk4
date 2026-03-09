#include "pages.h"
#include "widgets/container.h"
#include "widgets/toggle.h"

extern void on_checkbox_toggled(GtkCheckButton *button, gpointer user_data);
extern void on_switch_changed(GtkSwitch *sw, GParamSpec *pspec, gpointer user_data);

GtkWidget *make_toggles_page(showcase_state *state) {
  GtkWidget *box = create_box(&(box_config){
      .orientation = GTK_ORIENTATION_VERTICAL,
      .spacing = 12,
      .homogeneous = false,
      .style = {.margin_top = 16,
                .margin_bottom = 16,
                .margin_start = 16,
                .margin_end = 16},
  });

  container_add(box, create_checkbox(&(checkbox_config){
                                   .label = "Enable notifications",
                                   .active = false,
                                   .inconsistent = false,
                                   .style = {.margin_bottom = 6},
                                   .on_toggled = on_checkbox_toggled,
                                   .user_data = state,
                               }), 0, 0, 0, 0);

  GtkWidget *switch_row = create_switch_row(
      &(switch_config){
          .label = "Realtime sync",
          .active = true,
          .state = true,
          .style = {0},
          .on_active_changed = on_switch_changed,
          .user_data = state,
      },
      &state->switch_widget);
  container_add(box, switch_row, 0, 0, 0, 0);
  return box;
}
