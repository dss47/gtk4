#include "pages.h"
#include "widgets/container.h"
#include "widgets/button.h"
#include "widgets/separator.h"
#include "widgets/display.h"

extern void on_icon_btn_clicked(GtkButton *button, gpointer user_data);
extern void on_copy_clicked(GtkButton *button, gpointer user_data);
extern void on_home_clicked(GtkButton *button, gpointer user_data);
extern void on_search_clicked(GtkButton *button, gpointer user_data);
extern void on_star_clicked(GtkButton *button, gpointer user_data);

GtkWidget *make_toolbar_page(showcase_state *state) {
  GtkWidget *box = create_box(&(box_config){
      .orientation = GTK_ORIENTATION_VERTICAL,
      .spacing = 14,
      .homogeneous = false,
      .style = {.margin_top = 16,
                .margin_bottom = 16,
                .margin_start = 16,
                .margin_end = 16},
  });

  /* Icon-only toolbar */
  container_add(box, create_label(&(label_config){
      .text = "Icon-Only Toolbar",
      .style = {.css_class = "title-2", .margin_bottom = 4},
  }), 0, 0, 0, 0);

  GtkWidget *toolbar = create_box(&(box_config){
      .orientation = GTK_ORIENTATION_HORIZONTAL,
      .spacing = 4,
      .style = {.halign = GTK_ALIGN_CENTER, .set_halign = true},
  });
  container_add(toolbar, create_button(&(button_config){
      .icon_name = "document-new-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "New"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}), 0, 0, 0, 0);
  container_add(toolbar, create_button(&(button_config){
      .icon_name = "document-open-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Open"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}), 0, 0, 0, 0);
  container_add(toolbar, create_button(&(button_config){
      .icon_name = "document-save-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Save"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}), 0, 0, 0, 0);
  container_add(toolbar, create_separator(&(separator_config){
      .orientation = GTK_ORIENTATION_VERTICAL,
      .style = {.margin_start = 4, .margin_end = 4}}), 0, 0, 0, 0);
  container_add(toolbar, create_button(&(button_config){
      .icon_name = "edit-cut-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Cut"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}), 0, 0, 0, 0);
  container_add(toolbar, create_button(&(button_config){
      .icon_name = "edit-copy-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Copy"},
      .on_clicked = on_copy_clicked, .user_data = state}), 0, 0, 0, 0);
  container_add(toolbar, create_button(&(button_config){
      .icon_name = "edit-paste-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Paste"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}), 0, 0, 0, 0);
  container_add(toolbar, create_separator(&(separator_config){
      .orientation = GTK_ORIENTATION_VERTICAL,
      .style = {.margin_start = 4, .margin_end = 4}}), 0, 0, 0, 0);
  container_add(toolbar, create_button(&(button_config){
      .icon_name = "edit-undo-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Undo"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}), 0, 0, 0, 0);
  container_add(toolbar, create_button(&(button_config){
      .icon_name = "edit-redo-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Redo"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}), 0, 0, 0, 0);
  container_add(box, toolbar, 0, 0, 0, 0);

  container_add(box, create_separator(&(separator_config){
      .orientation = GTK_ORIENTATION_HORIZONTAL}), 0, 0, 0, 0);

  /* Labeled icon buttons */
  container_add(box, create_label(&(label_config){
      .text = "Labeled Icon Buttons",
      .style = {.css_class = "title-2", .margin_bottom = 4},
  }), 0, 0, 0, 0);

  GtkWidget *btn_grid = create_grid(&(grid_config){
      .column_spacing = 10, .row_spacing = 10,
      .style = {.halign = GTK_ALIGN_CENTER, .set_halign = true},
  });

  container_add(btn_grid, create_button(&(button_config){
      .label = "Home", .icon_name = "go-home-symbolic",
      .theme_variant = "primary", .has_frame = true,
      .icon_size = 16, .style = {.width_request = 130},
      .on_clicked = on_home_clicked, .user_data = state}), 0, 0, 1, 1);

  container_add(btn_grid, create_button(&(button_config){
      .label = "Search", .icon_name = "system-search-symbolic",
      .theme_variant = "primary", .has_frame = true,
      .icon_size = 16, .style = {.width_request = 130},
      .on_clicked = on_search_clicked, .user_data = state}), 1, 0, 1, 1);

  container_add(btn_grid, create_button(&(button_config){
      .label = "Star", .icon_name = "starred-symbolic",
      .theme_variant = "success", .has_frame = true,
      .icon_size = 16, .style = {.width_request = 130},
      .on_clicked = on_star_clicked, .user_data = state}), 0, 1, 1, 1);

  container_add(btn_grid, create_button(&(button_config){
      .label = "Copy", .icon_name = "edit-copy-symbolic",
      .theme_variant = "success", .has_frame = true,
      .icon_size = 16, .style = {.width_request = 130},
      .on_clicked = on_copy_clicked, .user_data = state}), 1, 1, 1, 1);

  container_add(btn_grid, create_button(&(button_config){
      .label = "Refresh", .icon_name = "view-refresh-symbolic",
      .theme_variant = "flat", .has_frame = false,
      .icon_size = 16, .style = {.width_request = 130},
      .on_clicked = on_icon_btn_clicked, .user_data = state}), 0, 2, 1, 1);

  container_add(btn_grid, create_button(&(button_config){
      .label = "Delete", .icon_name = "edit-delete-symbolic",
      .theme_variant = "danger", .has_frame = true,
      .icon_size = 16, .style = {.width_request = 130},
      .on_clicked = on_icon_btn_clicked, .user_data = state}), 1, 2, 1, 1);

  container_add(box, btn_grid, 0, 0, 0, 0);

  container_add(box, create_separator(&(separator_config){
      .orientation = GTK_ORIENTATION_HORIZONTAL}), 0, 0, 0, 0);

  /* Navigation row */
  container_add(box, create_label(&(label_config){
      .text = "Navigation",
      .style = {.css_class = "title-2", .margin_bottom = 4},
  }), 0, 0, 0, 0);
  GtkWidget *nav = create_box(&(box_config){
      .orientation = GTK_ORIENTATION_HORIZONTAL,
      .spacing = 6,
      .style = {.halign = GTK_ALIGN_CENTER, .set_halign = true},
  });
  container_add(nav, create_button(&(button_config){
      .icon_name = "go-previous-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Back"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}), 0, 0, 0, 0);
  container_add(nav, create_button(&(button_config){
      .icon_name = "go-home-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Home"},
      .on_clicked = on_home_clicked, .user_data = state}), 0, 0, 0, 0);
  container_add(nav, create_button(&(button_config){
      .icon_name = "go-next-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Forward"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}), 0, 0, 0, 0);
  container_add(nav, create_separator(&(separator_config){
      .orientation = GTK_ORIENTATION_VERTICAL,
      .style = {.margin_start = 4, .margin_end = 4}}), 0, 0, 0, 0);
  container_add(nav, create_button(&(button_config){
      .icon_name = "view-refresh-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Refresh"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}), 0, 0, 0, 0);
  container_add(nav, create_button(&(button_config){
      .icon_name = "view-fullscreen-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Fullscreen"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}), 0, 0, 0, 0);
  container_add(box, nav, 0, 0, 0, 0);

  return box;
}
