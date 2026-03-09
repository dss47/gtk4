#include "pages.h"
#include "widgets/container.h"
#include "widgets/display.h"
#include "widgets/button.h"

extern void on_inc_progress(GtkButton *button, gpointer user_data);
extern void on_toggle_spinner(GtkButton *button, gpointer user_data);

GtkWidget *make_displays_page(showcase_state *state) {
  GtkWidget *box = create_box(&(box_config){
      .orientation = GTK_ORIENTATION_VERTICAL,
      .spacing = 12,
      .homogeneous = false,
      .style = {.margin_top = 16,
                .margin_bottom = 16,
                .margin_start = 16,
                .margin_end = 16},
  });

  state->progress = create_progress_bar(&(progress_bar_config){
      .fraction = 0.32,
      .text = "Working",
      .orientation = GTK_ORIENTATION_HORIZONTAL,
      .show_text = true,
      .inverted = false,
      .pulse_once = false,
      .style = {.width_request = 340},
  });
  state->spinner = create_spinner(&(spinner_config){
      .size = 26,
      .spinning = true,
      .style = {0},
  });
  state->spinner_running = true;

  container_add(box, state->progress, 0, 0, 0, 0);
  container_add(box, state->spinner, 0, 0, 0, 0);
  container_add(box, create_button(&(button_config){
                                   .label = "Advance progress",
                                   .icon_name = "go-next-symbolic",
                                   .theme_variant = "primary",
                                   .use_underline = false,
                                   .has_frame = true,
                                   .can_shrink = true,
                                   .icon_size = 16,
                                   .style = {.width_request = 220},
                                   .on_clicked = on_inc_progress,
                                   .user_data = state,
                               }), 0, 0, 0, 0);
  container_add(box, create_button(&(button_config){
                                   .label = "Toggle spinner",
                                   .icon_name = "view-refresh-symbolic",
                                   .theme_variant = "flat",
                                   .use_underline = false,
                                   .has_frame = false,
                                   .can_shrink = true,
                                   .icon_size = 16,
                                   .style = {.width_request = 220},
                                   .on_clicked = on_toggle_spinner,
                                   .user_data = state,
                               }), 0, 0, 0, 0);
  return box;
}
