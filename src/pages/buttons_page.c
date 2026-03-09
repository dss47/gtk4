#include "pages.h"
#include "widgets/container.h"
#include "widgets/button.h"

extern void on_button_clicked(GtkButton *button, gpointer user_data);

GtkWidget *make_buttons_page(showcase_state *state) {
  GtkWidget *box = create_box(&(box_config){
      .orientation = GTK_ORIENTATION_VERTICAL,
      .spacing = 10,
      .homogeneous = false,
      .style = {.margin_top = 16,
                .margin_bottom = 16,
                .margin_start = 16,
                .margin_end = 16},
  });

  container_add(box,
               create_button(&(button_config){
                   .label = "_Save",
                   .icon_name = "document-save-symbolic",
                   .theme_variant = "primary",
                   .use_underline = true,
                   .has_frame = true,
                   .can_shrink = false,
                   .icon_size = 16,
                   .style = {.width_request = 180, .height_request = 38},
                   .on_clicked = on_button_clicked,
                   .user_data = state,
               }), 0, 0, 0, 0);

  container_add(box,
               create_button(&(button_config){
                   .label = "Approve",
                   .icon_name = "emblem-ok-symbolic",
                   .theme_variant = "success",
                   .use_underline = false,
                   .has_frame = true,
                   .can_shrink = true,
                   .icon_size = 16,
                   .style = {.width_request = 180, .height_request = 38},
                   .on_clicked = on_button_clicked,
                   .user_data = state,
               }), 0, 0, 0, 0);

  container_add(box,
               create_button(&(button_config){
                   .label = "Delete",
                   .icon_name = "user-trash-symbolic",
                   .theme_variant = "danger",
                   .use_underline = false,
                   .has_frame = true,
                   .can_shrink = true,
                   .icon_size = 16,
                   .style = {.width_request = 180, .height_request = 38},
                   .on_clicked = on_button_clicked,
                   .user_data = state,
               }), 0, 0, 0, 0);

  GtkWidget *radio_a = create_radio_button(&(radio_button_config){
      .label = "Mode A",
      .group_with = NULL,
      .is_active = true,
      .style = {0},
      .on_toggled = NULL,
      .user_data = NULL,
  });
  GtkWidget *radio_b = create_radio_button(&(radio_button_config){
      .label = "Mode B",
      .group_with = radio_a,
      .is_active = false,
      .style = {0},
      .on_toggled = NULL,
      .user_data = NULL,
  });
  container_add(box, radio_a, 0, 0, 0, 0);
  container_add(box, radio_b, 0, 0, 0, 0);
  return box;
}
