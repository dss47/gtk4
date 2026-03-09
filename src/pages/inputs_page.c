#include "pages.h"
#include "widgets/container.h"
#include "widgets/input.h"
#include "widgets/display.h"

extern void on_entry_changed(GtkEditable *editable, gpointer user_data);
extern void on_role_selected(GtkDropDown *dropdown, GParamSpec *pspec, gpointer user_data);
extern void on_spin_value_changed(GtkSpinButton *spin, gpointer user_data);

GtkWidget *make_inputs_page(showcase_state *state) {
  GtkWidget *grid = create_grid(&(grid_config){
      .column_spacing = 12,
      .row_spacing = 12,
      .homogeneous_columns = false,
      .homogeneous_rows = false,
      .style = {.margin_top = 16,
                .margin_bottom = 16,
                .margin_start = 16,
                .margin_end = 16},
  });

  const char *roles[] = {"Admin", "Teacher", "Student", "Guest", NULL};
  GtkWidget *label_user = create_label(&(label_config){
      .text = "Username",
      .selectable = false,
      .wrap = false,
      .ellipsize = PANGO_ELLIPSIZE_NONE,
      .max_width_chars = 0,
      .style = {0},
  });
  GtkWidget *entry_user = create_entry(&(entry_config){
      .placeholder = "enter username",
      .default_text = "",
      .purpose_hint = "text",
      .password = false,
      .editable = true,
      .max_length = 60,
      .style = {.hexpand = true, .set_hexpand = true},
      .on_changed = on_entry_changed,
      .user_data = state,
  });

  GtkWidget *label_pass = create_label(&(label_config){
      .text = "Password",
      .selectable = false,
      .wrap = false,
      .ellipsize = PANGO_ELLIPSIZE_NONE,
      .max_width_chars = 0,
      .style = {0},
  });
  GtkWidget *entry_pass = create_entry(&(entry_config){
      .placeholder = "enter password",
      .default_text = "",
      .purpose_hint = "text",
      .password = true,
      .editable = true,
      .max_length = 60,
      .style = {.hexpand = true, .set_hexpand = true},
      .on_changed = on_entry_changed,
      .user_data = state,
  });

  GtkWidget *dropdown = create_dropdown(&(dropdown_config){
      .options = roles,
      .selected_index = 0,
      .enable_search = true,
      .style = {.hexpand = true, .set_hexpand = true},
      .on_selected = on_role_selected,
      .user_data = state,
  });

  GtkWidget *spin = create_spin_button(&(spin_button_config){
      .min_value = 0.0,
      .max_value = 100.0,
      .step = 0.5,
      .page = 5.0,
      .value = 12.5,
      .digits = 1,
      .numeric_only = true,
      .wrap = false,
      .style = {.width_request = 140},
      .on_value_changed = on_spin_value_changed,
      .user_data = state,
  });

  container_add(grid, label_user, 0, 0, 1, 1);
  container_add(grid, entry_user, 1, 0, 1, 1);
  container_add(grid, label_pass, 0, 1, 1, 1);
  container_add(grid, entry_pass, 1, 1, 1, 1);
  container_add(grid,
                create_label(&(label_config){.text = "Role", .style = {0}}),
                0, 2, 1, 1);
  container_add(grid, dropdown, 1, 2, 1, 1);
  container_add(grid,
                create_label(&(label_config){.text = "Score", .style = {0}}),
                0, 3, 1, 1);
  container_add(grid, spin, 1, 3, 1, 1);
  return grid;
}
