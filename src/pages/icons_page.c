#include "pages.h"
#include "widgets/container.h"
#include "widgets/display.h"
#include "widgets/input.h"
#include "widgets/button.h"
#include "widgets/separator.h"

// Note: assuming handlers are accessible. In C, they need to be declared or included.
// Since src/demo_app.c included src/handlers.c, they were effectively global.
// I'll need to figure out how to make 'on_*' handlers visible here.
// For now, I'll include the widget headers.

extern void on_icon_selected(GtkDropDown *dropdown, GParamSpec *pspec, gpointer user_data);
extern void on_zoom_out(GtkButton *button, gpointer user_data);
extern void on_zoom_in(GtkButton *button, gpointer user_data);

GtkWidget *make_icons_page(showcase_state *state) {
  GtkWidget *box = create_box(&(box_config){
      .orientation = GTK_ORIENTATION_VERTICAL,
      .spacing = 12,
      .homogeneous = false,
      .style = {.margin_top = 16,
                .margin_bottom = 16,
                .margin_start = 16,
                .margin_end = 16},
  });

  /* Row of themed icons at different sizes */
  container_add(box, create_label(&(label_config){
      .text = "System Icons",
      .style = {.css_class = "title-2", .margin_bottom = 4},
  }), 0, 0, 0, 0);

  GtkWidget *icon_row = create_box(&(box_config){
      .orientation = GTK_ORIENTATION_HORIZONTAL,
      .spacing = 16,
      .homogeneous = false,
      .style = {.halign = GTK_ALIGN_CENTER, .set_halign = true},
  });
  container_add(icon_row, create_image(&(image_config){
      .icon_name = "dialog-information-symbolic", .pixel_size = 32,
      .style = {.tooltip = "Information"}}), 0, 0, 0, 0);
  container_add(icon_row, create_image(&(image_config){
      .icon_name = "dialog-warning-symbolic", .pixel_size = 32,
      .style = {.tooltip = "Warning"}}), 0, 0, 0, 0);
  container_add(icon_row, create_image(&(image_config){
      .icon_name = "dialog-error-symbolic", .pixel_size = 32,
      .style = {.tooltip = "Error"}}), 0, 0, 0, 0);
  container_add(icon_row, create_image(&(image_config){
      .icon_name = "emblem-favorite-symbolic", .pixel_size = 32,
      .style = {.tooltip = "Favorite"}}), 0, 0, 0, 0);
  container_add(icon_row, create_image(&(image_config){
      .icon_name = "face-smile-symbolic", .pixel_size = 32,
      .style = {.tooltip = "Smile"}}), 0, 0, 0, 0);
  container_add(icon_row, create_image(&(image_config){
      .icon_name = "weather-clear-symbolic", .pixel_size = 32,
      .style = {.tooltip = "Weather"}}), 0, 0, 0, 0);
  container_add(box, icon_row, 0, 0, 0, 0);

  container_add(box, create_separator(&(separator_config){
      .orientation = GTK_ORIENTATION_HORIZONTAL}), 0, 0, 0, 0);

  /* Interactive icon preview with zoom */
  container_add(box, create_label(&(label_config){
      .text = "Interactive Icon Preview",
      .style = {.css_class = "title-2", .margin_bottom = 4},
  }), 0, 0, 0, 0);

  state->icon_preview = create_image(&(image_config){
      .icon_name = "applications-development-symbolic",
      .pixel_size = 48,
      .style = {.halign = GTK_ALIGN_CENTER, .set_halign = true},
  });
  state->icon_size_label = create_label(&(label_config){
      .text = "48 px",
      .style = {.halign = GTK_ALIGN_CENTER, .set_halign = true},
  });

  const char *icon_choices[] = {
      "Development", "Smile", "Weather",
      "Warning", "Favorite", "Audio", NULL};
  GtkWidget *icon_picker = create_dropdown(&(dropdown_config){
      .options = icon_choices,
      .selected_index = 0,
      .enable_search = false,
      .style = {.width_request = 200,
                .halign = GTK_ALIGN_CENTER, .set_halign = true},
      .on_selected = on_icon_selected,
      .user_data = state,
  });

  GtkWidget *zoom_row = create_box(&(box_config){
      .orientation = GTK_ORIENTATION_HORIZONTAL,
      .spacing = 8,
      .style = {.halign = GTK_ALIGN_CENTER, .set_halign = true},
  });
  container_add(zoom_row, create_button(&(button_config){
      .label = NULL, .icon_name = "zoom-out-symbolic",
      .theme_variant = "flat", .has_frame = false, .icon_size = 16,
      .style = {.tooltip = "Zoom Out"},
      .on_clicked = on_zoom_out, .user_data = state}), 0, 0, 0, 0);
  container_add(zoom_row, state->icon_size_label, 0, 0, 0, 0);
  container_add(zoom_row, create_button(&(button_config){
      .label = NULL, .icon_name = "zoom-in-symbolic",
      .theme_variant = "flat", .has_frame = false, .icon_size = 16,
      .style = {.tooltip = "Zoom In"},
      .on_clicked = on_zoom_in, .user_data = state}), 0, 0, 0, 0);

  container_add(box, state->icon_preview, 0, 0, 0, 0);
  container_add(box, zoom_row, 0, 0, 0, 0);
  container_add(box, icon_picker, 0, 0, 0, 0);
  return box;
}
