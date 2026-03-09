#include "demo_app.h"

#include "gtk/gtk.h"
#include "widgets/button.h"
#include "widgets/container.h"
#include "widgets/date.h"
#include "widgets/dialog.h"
#include "widgets/display.h"
#include "widgets/input.h"
#include "widgets/menu.h"
#include "widgets/separator.h"
#include "widgets/theme.h"
#include "widgets/toggle.h"
#include "widgets/window.h"

#include <string.h>

typedef struct {
  GtkWidget *status_label;
  GtkWidget *progress;
  GtkWidget *spinner;
  GtkWidget *switch_widget;
  GtkWidget *icon_size_label;
  GtkWidget *icon_preview;
  int icon_counter;
  bool spinner_running;
} showcase_state;

static void set_status(showcase_state *state, const char *text) {
  gtk_label_set_text(GTK_LABEL(state->status_label), text);
}

static void on_entry_changed(GtkEditable *editable, gpointer user_data) {
  showcase_state *state = user_data;
  char msg[96];
  g_snprintf(msg, sizeof(msg), "Input length: %zu",
             strlen(gtk_editable_get_text(editable)));
  set_status(state, msg);
}

static void on_role_selected(GtkDropDown *dropdown, GParamSpec *pspec,
                             gpointer user_data) {
  (void)pspec;
  showcase_state *state = user_data;
  char msg[96];
  g_snprintf(msg, sizeof(msg), "Role selected index: %u",
             gtk_drop_down_get_selected(dropdown));
  set_status(state, msg);
}

static void on_spin_value_changed(GtkSpinButton *spin, gpointer user_data) {
  showcase_state *state = user_data;
  char msg[96];
  g_snprintf(msg, sizeof(msg), "Spin value: %.2f",
             gtk_spin_button_get_value(spin));
  set_status(state, msg);
}

static void on_button_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  set_status((showcase_state *)user_data, "Primary action clicked");
}

static void on_checkbox_toggled(GtkCheckButton *button, gpointer user_data) {
  showcase_state *state = user_data;
  set_status(state, gtk_check_button_get_active(button) ? "Checkbox enabled"
                                                        : "Checkbox disabled");
}

static void on_switch_changed(GtkSwitch *sw, GParamSpec *pspec,
                              gpointer user_data) {
  (void)pspec;
  showcase_state *state = user_data;
  set_status(state,
             gtk_switch_get_active(sw) ? "Switch active" : "Switch inactive");
}

static void on_day_selected(GtkCalendar *calendar, gpointer user_data) {
  showcase_state *state = user_data;
  GDateTime *d = gtk_calendar_get_date(calendar);
  char *s = g_date_time_format(d, "%Y-%m-%d");
  char msg[128];
  g_snprintf(msg, sizeof(msg), "Date selected: %s", s);
  set_status(state, msg);
  g_free(s);
  g_date_time_unref(d);
}

static void on_alert_result(int response_index, gpointer user_data) {
  showcase_state *state = user_data;
  char msg[96];
  g_snprintf(msg, sizeof(msg), "Dialog response index: %d", response_index);
  set_status(state, msg);
}

static void on_show_dialog(GtkButton *button, gpointer user_data) {
  (void)button;
  showcase_state *state = user_data;
  GtkRoot *root = gtk_widget_get_root(state->status_label);
  alert_button_config buttons[] = {
      {.label = "Cancel", .appearance = "flat"},
      {.label = "Confirm", .appearance = "suggested"},
  };

  show_alert_dialog(&(alert_dialog_config){
      .parent = (root && GTK_IS_WINDOW(root)) ? GTK_WINDOW(root) : NULL,
      .message = "Proceed with wrapper-driven action?",
      .detail = "This dialog uses GtkAlertDialog only (no deprecated API).",
      .buttons = buttons,
      .button_count = G_N_ELEMENTS(buttons),
      .default_button = 1,
      .cancel_button = 0,
      .modal = true,
      .on_response = on_alert_result,
      .user_data = state,
  });
}

static void on_inc_progress(GtkButton *button, gpointer user_data) {
  (void)button;
  showcase_state *state = user_data;
  double f = gtk_progress_bar_get_fraction(GTK_PROGRESS_BAR(state->progress));
  f += 0.12;
  if (f > 1.0) {
    f = 0.0;
  }
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(state->progress), f);
  gtk_progress_bar_set_text(GTK_PROGRESS_BAR(state->progress),
                            f > 0.99 ? "Done" : "Working");
  set_status(state, "Progress updated");
}

static void on_toggle_spinner(GtkButton *button, gpointer user_data) {
  (void)button;
  showcase_state *state = user_data;
  state->spinner_running = !state->spinner_running;
  if (state->spinner_running) {
    gtk_spinner_start(GTK_SPINNER(state->spinner));
    set_status(state, "Spinner started");
  } else {
    gtk_spinner_stop(GTK_SPINNER(state->spinner));
    set_status(state, "Spinner stopped");
  }
}

static void on_icon_btn_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  showcase_state *state = user_data;
  state->icon_counter++;
  char msg[96];
  g_snprintf(msg, sizeof(msg), "Icon button clicked %d time(s)", state->icon_counter);
  set_status(state, msg);
}

static void on_star_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  set_status((showcase_state *)user_data, "Starred!");
}

static void on_copy_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  set_status((showcase_state *)user_data, "Copied to clipboard");
}

static void on_search_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  set_status((showcase_state *)user_data, "Search activated");
}

static void on_home_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  set_status((showcase_state *)user_data, "Navigated home");
}

static void on_zoom_in(GtkButton *button, gpointer user_data) {
  (void)button;
  showcase_state *state = user_data;
  int size = gtk_image_get_pixel_size(GTK_IMAGE(state->icon_preview));
  if (size < 128) {
    size += 8;
    gtk_image_set_pixel_size(GTK_IMAGE(state->icon_preview), size);
  }
  char msg[64];
  g_snprintf(msg, sizeof(msg), "%d px", size);
  gtk_label_set_text(GTK_LABEL(state->icon_size_label), msg);
  set_status(state, "Zoomed in");
}

static void on_zoom_out(GtkButton *button, gpointer user_data) {
  (void)button;
  showcase_state *state = user_data;
  int size = gtk_image_get_pixel_size(GTK_IMAGE(state->icon_preview));
  if (size > 16) {
    size -= 8;
    gtk_image_set_pixel_size(GTK_IMAGE(state->icon_preview), size);
  }
  char msg[64];
  g_snprintf(msg, sizeof(msg), "%d px", size);
  gtk_label_set_text(GTK_LABEL(state->icon_size_label), msg);
  set_status(state, "Zoomed out");
}

static void on_icon_selected(GtkDropDown *dropdown, GParamSpec *pspec,
                             gpointer user_data) {
  (void)pspec;
  showcase_state *state = user_data;
  static const char *icons[] = {
      "applications-development-symbolic", "face-smile-symbolic",
      "weather-clear-symbolic", "dialog-warning-symbolic",
      "emblem-favorite-symbolic", "audio-volume-high-symbolic"};
  guint idx = gtk_drop_down_get_selected(dropdown);
  if (idx < G_N_ELEMENTS(icons)) {
    gtk_image_set_from_icon_name(GTK_IMAGE(state->icon_preview), icons[idx]);
    char msg[96];
    g_snprintf(msg, sizeof(msg), "Icon changed to: %s", icons[idx]);
    set_status(state, msg);
  }
}

static GtkWidget *make_icons_page(showcase_state *state) {
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
  gtk_box_append(GTK_BOX(box), create_label(&(label_config){
      .text = "System Icons",
      .style = {.css_class = "title-2", .margin_bottom = 4},
  }));

  GtkWidget *icon_row = create_box(&(box_config){
      .orientation = GTK_ORIENTATION_HORIZONTAL,
      .spacing = 16,
      .homogeneous = false,
      .style = {.halign = GTK_ALIGN_CENTER, .set_halign = true},
  });
  gtk_box_append(GTK_BOX(icon_row), create_image(&(image_config){
      .icon_name = "dialog-information-symbolic", .pixel_size = 32,
      .style = {.tooltip = "Information"}}));
  gtk_box_append(GTK_BOX(icon_row), create_image(&(image_config){
      .icon_name = "dialog-warning-symbolic", .pixel_size = 32,
      .style = {.tooltip = "Warning"}}));
  gtk_box_append(GTK_BOX(icon_row), create_image(&(image_config){
      .icon_name = "dialog-error-symbolic", .pixel_size = 32,
      .style = {.tooltip = "Error"}}));
  gtk_box_append(GTK_BOX(icon_row), create_image(&(image_config){
      .icon_name = "emblem-favorite-symbolic", .pixel_size = 32,
      .style = {.tooltip = "Favorite"}}));
  gtk_box_append(GTK_BOX(icon_row), create_image(&(image_config){
      .icon_name = "face-smile-symbolic", .pixel_size = 32,
      .style = {.tooltip = "Smile"}}));
  gtk_box_append(GTK_BOX(icon_row), create_image(&(image_config){
      .icon_name = "weather-clear-symbolic", .pixel_size = 32,
      .style = {.tooltip = "Weather"}}));
  gtk_box_append(GTK_BOX(box), icon_row);

  gtk_box_append(GTK_BOX(box), create_separator(&(separator_config){
      .orientation = GTK_ORIENTATION_HORIZONTAL}));

  /* Interactive icon preview with zoom */
  gtk_box_append(GTK_BOX(box), create_label(&(label_config){
      .text = "Interactive Icon Preview",
      .style = {.css_class = "title-2", .margin_bottom = 4},
  }));

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
  gtk_box_append(GTK_BOX(zoom_row), create_button(&(button_config){
      .label = NULL, .icon_name = "zoom-out-symbolic",
      .theme_variant = "flat", .has_frame = false, .icon_size = 16,
      .style = {.tooltip = "Zoom Out"},
      .on_clicked = on_zoom_out, .user_data = state}));
  gtk_box_append(GTK_BOX(zoom_row), state->icon_size_label);
  gtk_box_append(GTK_BOX(zoom_row), create_button(&(button_config){
      .label = NULL, .icon_name = "zoom-in-symbolic",
      .theme_variant = "flat", .has_frame = false, .icon_size = 16,
      .style = {.tooltip = "Zoom In"},
      .on_clicked = on_zoom_in, .user_data = state}));

  gtk_box_append(GTK_BOX(box), state->icon_preview);
  gtk_box_append(GTK_BOX(box), zoom_row);
  gtk_box_append(GTK_BOX(box), icon_picker);
  return box;
}

static GtkWidget *make_toolbar_page(showcase_state *state) {
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
  gtk_box_append(GTK_BOX(box), create_label(&(label_config){
      .text = "Icon-Only Toolbar",
      .style = {.css_class = "title-2", .margin_bottom = 4},
  }));

  GtkWidget *toolbar = create_box(&(box_config){
      .orientation = GTK_ORIENTATION_HORIZONTAL,
      .spacing = 4,
      .style = {.halign = GTK_ALIGN_CENTER, .set_halign = true},
  });
  gtk_box_append(GTK_BOX(toolbar), create_button(&(button_config){
      .icon_name = "document-new-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "New"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}));
  gtk_box_append(GTK_BOX(toolbar), create_button(&(button_config){
      .icon_name = "document-open-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Open"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}));
  gtk_box_append(GTK_BOX(toolbar), create_button(&(button_config){
      .icon_name = "document-save-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Save"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}));
  gtk_box_append(GTK_BOX(toolbar), create_separator(&(separator_config){
      .orientation = GTK_ORIENTATION_VERTICAL,
      .style = {.margin_start = 4, .margin_end = 4}}));
  gtk_box_append(GTK_BOX(toolbar), create_button(&(button_config){
      .icon_name = "edit-cut-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Cut"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}));
  gtk_box_append(GTK_BOX(toolbar), create_button(&(button_config){
      .icon_name = "edit-copy-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Copy"},
      .on_clicked = on_copy_clicked, .user_data = state}));
  gtk_box_append(GTK_BOX(toolbar), create_button(&(button_config){
      .icon_name = "edit-paste-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Paste"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}));
  gtk_box_append(GTK_BOX(toolbar), create_separator(&(separator_config){
      .orientation = GTK_ORIENTATION_VERTICAL,
      .style = {.margin_start = 4, .margin_end = 4}}));
  gtk_box_append(GTK_BOX(toolbar), create_button(&(button_config){
      .icon_name = "edit-undo-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Undo"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}));
  gtk_box_append(GTK_BOX(toolbar), create_button(&(button_config){
      .icon_name = "edit-redo-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Redo"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}));
  gtk_box_append(GTK_BOX(box), toolbar);

  gtk_box_append(GTK_BOX(box), create_separator(&(separator_config){
      .orientation = GTK_ORIENTATION_HORIZONTAL}));

  /* Labeled icon buttons */
  gtk_box_append(GTK_BOX(box), create_label(&(label_config){
      .text = "Labeled Icon Buttons",
      .style = {.css_class = "title-2", .margin_bottom = 4},
  }));

  GtkWidget *btn_grid = create_grid(&(grid_config){
      .column_spacing = 10, .row_spacing = 10,
      .style = {.halign = GTK_ALIGN_CENTER, .set_halign = true},
  });

  gtk_grid_attach(GTK_GRID(btn_grid), create_button(&(button_config){
      .label = "Home", .icon_name = "go-home-symbolic",
      .theme_variant = "primary", .has_frame = true,
      .icon_size = 16, .style = {.width_request = 130},
      .on_clicked = on_home_clicked, .user_data = state}), 0, 0, 1, 1);

  gtk_grid_attach(GTK_GRID(btn_grid), create_button(&(button_config){
      .label = "Search", .icon_name = "system-search-symbolic",
      .theme_variant = "primary", .has_frame = true,
      .icon_size = 16, .style = {.width_request = 130},
      .on_clicked = on_search_clicked, .user_data = state}), 1, 0, 1, 1);

  gtk_grid_attach(GTK_GRID(btn_grid), create_button(&(button_config){
      .label = "Star", .icon_name = "starred-symbolic",
      .theme_variant = "success", .has_frame = true,
      .icon_size = 16, .style = {.width_request = 130},
      .on_clicked = on_star_clicked, .user_data = state}), 0, 1, 1, 1);

  gtk_grid_attach(GTK_GRID(btn_grid), create_button(&(button_config){
      .label = "Copy", .icon_name = "edit-copy-symbolic",
      .theme_variant = "success", .has_frame = true,
      .icon_size = 16, .style = {.width_request = 130},
      .on_clicked = on_copy_clicked, .user_data = state}), 1, 1, 1, 1);

  gtk_grid_attach(GTK_GRID(btn_grid), create_button(&(button_config){
      .label = "Refresh", .icon_name = "view-refresh-symbolic",
      .theme_variant = "flat", .has_frame = false,
      .icon_size = 16, .style = {.width_request = 130},
      .on_clicked = on_icon_btn_clicked, .user_data = state}), 0, 2, 1, 1);

  gtk_grid_attach(GTK_GRID(btn_grid), create_button(&(button_config){
      .label = "Delete", .icon_name = "edit-delete-symbolic",
      .theme_variant = "danger", .has_frame = true,
      .icon_size = 16, .style = {.width_request = 130},
      .on_clicked = on_icon_btn_clicked, .user_data = state}), 1, 2, 1, 1);

  gtk_box_append(GTK_BOX(box), btn_grid);

  gtk_box_append(GTK_BOX(box), create_separator(&(separator_config){
      .orientation = GTK_ORIENTATION_HORIZONTAL}));

  /* Navigation row */
  gtk_box_append(GTK_BOX(box), create_label(&(label_config){
      .text = "Navigation",
      .style = {.css_class = "title-2", .margin_bottom = 4},
  }));
  GtkWidget *nav = create_box(&(box_config){
      .orientation = GTK_ORIENTATION_HORIZONTAL,
      .spacing = 6,
      .style = {.halign = GTK_ALIGN_CENTER, .set_halign = true},
  });
  gtk_box_append(GTK_BOX(nav), create_button(&(button_config){
      .icon_name = "go-previous-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Back"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}));
  gtk_box_append(GTK_BOX(nav), create_button(&(button_config){
      .icon_name = "go-home-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Home"},
      .on_clicked = on_home_clicked, .user_data = state}));
  gtk_box_append(GTK_BOX(nav), create_button(&(button_config){
      .icon_name = "go-next-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Forward"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}));
  gtk_box_append(GTK_BOX(nav), create_separator(&(separator_config){
      .orientation = GTK_ORIENTATION_VERTICAL,
      .style = {.margin_start = 4, .margin_end = 4}}));
  gtk_box_append(GTK_BOX(nav), create_button(&(button_config){
      .icon_name = "view-refresh-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Refresh"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}));
  gtk_box_append(GTK_BOX(nav), create_button(&(button_config){
      .icon_name = "view-fullscreen-symbolic", .has_frame = true,
      .icon_size = 20, .style = {.tooltip = "Fullscreen"},
      .on_clicked = on_icon_btn_clicked, .user_data = state}));
  gtk_box_append(GTK_BOX(box), nav);

  return box;
}

static GtkWidget *make_inputs_page(showcase_state *state) {
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

  gtk_grid_attach(GTK_GRID(grid), label_user, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), entry_user, 1, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), label_pass, 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), entry_pass, 1, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid),
                  create_label(&(label_config){.text = "Role", .style = {0}}),
                  0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), dropdown, 1, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(grid),
                  create_label(&(label_config){.text = "Score", .style = {0}}),
                  0, 3, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), spin, 1, 3, 1, 1);
  return grid;
}

static GtkWidget *make_buttons_page(showcase_state *state) {
  GtkWidget *box = create_box(&(box_config){
      .orientation = GTK_ORIENTATION_VERTICAL,
      .spacing = 10,
      .homogeneous = false,
      .style = {.margin_top = 16,
                .margin_bottom = 16,
                .margin_start = 16,
                .margin_end = 16},
  });

  gtk_box_append(GTK_BOX(box),
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
                 }));

  gtk_box_append(GTK_BOX(box),
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
                 }));

  gtk_box_append(GTK_BOX(box),
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
                 }));

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
  gtk_box_append(GTK_BOX(box), radio_a);
  gtk_box_append(GTK_BOX(box), radio_b);
  return box;
}

static GtkWidget *make_toggles_page(showcase_state *state) {
  GtkWidget *box = create_box(&(box_config){
      .orientation = GTK_ORIENTATION_VERTICAL,
      .spacing = 12,
      .homogeneous = false,
      .style = {.margin_top = 16,
                .margin_bottom = 16,
                .margin_start = 16,
                .margin_end = 16},
  });

  gtk_box_append(GTK_BOX(box), create_checkbox(&(checkbox_config){
                                   .label = "Enable notifications",
                                   .active = false,
                                   .inconsistent = false,
                                   .style = {.margin_bottom = 6},
                                   .on_toggled = on_checkbox_toggled,
                                   .user_data = state,
                               }));

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
  gtk_box_append(GTK_BOX(box), switch_row);
  return box;
}

static GtkWidget *make_displays_page(showcase_state *state) {
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

  gtk_box_append(GTK_BOX(box), state->progress);
  gtk_box_append(GTK_BOX(box), state->spinner);
  gtk_box_append(GTK_BOX(box), create_button(&(button_config){
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
                               }));
  gtk_box_append(GTK_BOX(box), create_button(&(button_config){
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
                               }));
  return box;
}

static GtkWidget *make_dialogs_page(showcase_state *state) {
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

  gtk_box_append(GTK_BOX(box), create_button(&(button_config){
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
                               }));

  gtk_box_append(GTK_BOX(box), create_calendar(&(calendar_config){
                                   .selected_date = now,
                                   .show_day_names = true,
                                   .show_week_numbers = true,
                                   .no_month_change = false,
                                   .style = {.margin_top = 8},
                                   .on_day_selected = on_day_selected,
                                   .user_data = state,
                               }));
  g_date_time_unref(now);
  return box;
}

static void on_menu_about(GSimpleAction *action, GVariant *parameter,
                          gpointer user_data) {
  (void)action;
  (void)parameter;
  GtkApplication *app = GTK_APPLICATION(user_data);
  GtkWindow *parent = gtk_application_get_active_window(app);

  show_alert_dialog(&(alert_dialog_config){
      .parent = parent,
      .message = "GTK UI Wrapper Showcase",
      .detail = "Menu actions now use direct callbacks from menu structs.",
      .buttons = NULL,
      .button_count = 0,
      .default_button = -1,
      .cancel_button = -1,
      .modal = true,
      .on_response = NULL,
      .user_data = NULL,
  });
}

static void on_menu_quit(GSimpleAction *action, GVariant *parameter,
                         gpointer user_data) {
  (void)action;
  (void)parameter;
  g_application_quit(G_APPLICATION(user_data));
}

static GtkWidget *build_showcase(GtkApplication *app, GtkWindow *window,
                                 showcase_state *state) {
  (void)window;
  GtkWidget *root = create_box(&(box_config){
      .orientation = GTK_ORIENTATION_VERTICAL,
      .spacing = 0,
      .homogeneous = false,
      .style = {.hexpand = true,
                .set_hexpand = true,
                .vexpand = true,
                .set_vexpand = true},
  });

  const menu_item_config recent_children[] = {
      {.label = "Project A",
       .kind = MENU_ITEM_ACTION,
       .children = NULL,
       .child_count = 0,
       .enabled = true,
       .on_activate = on_menu_about,
       .user_data = app},
      {.label = "Project B",
       .kind = MENU_ITEM_ACTION,
       .children = NULL,
       .child_count = 0,
       .enabled = true,
       .on_activate = on_menu_about,
       .user_data = app},
  };
  const menu_item_config export_children[] = {
      {.label = "PNG",
       .kind = MENU_ITEM_ACTION,
       .children = NULL,
       .child_count = 0,
       .enabled = true,
       .on_activate = on_menu_about,
       .user_data = app},
      {.label = "PDF",
       .kind = MENU_ITEM_ACTION,
       .children = NULL,
       .child_count = 0,
       .enabled = true,
       .on_activate = on_menu_about,
       .user_data = app},
  };
  const menu_item_config file_items[] = {
      {.label = "New",
       .kind = MENU_ITEM_ACTION,
       .children = NULL,
       .child_count = 0,
       .enabled = true,
       .on_activate = on_menu_about,
       .user_data = app},
      {.label = "Open",
       .kind = MENU_ITEM_ACTION,
       .children = NULL,
       .child_count = 0,
       .enabled = true,
       .on_activate = on_menu_about,
       .user_data = app},
      {.label = "Recent",
       .kind = MENU_ITEM_SUBMENU,
       .children = recent_children,
       .child_count = G_N_ELEMENTS(recent_children),
       .enabled = true,
       .on_activate = NULL,
       .user_data = NULL},
      {.label = "Export",
       .kind = MENU_ITEM_SUBMENU,
       .children = export_children,
       .child_count = G_N_ELEMENTS(export_children),
       .enabled = true,
       .on_activate = NULL,
       .user_data = NULL},
      {.label = "Quit",
       .kind = MENU_ITEM_ACTION,
       .children = NULL,
       .child_count = 0,
       .enabled = true,
       .on_activate = on_menu_quit,
       .user_data = app},
  };
  const menu_item_config help_items[] = {
      {.label = "About",
       .kind = MENU_ITEM_ACTION,
       .children = NULL,
       .child_count = 0,
       .enabled = true,
       .on_activate = on_menu_about,
       .user_data = app},
  };
  const menu_section_config sections[] = {
      {.label = "File",
       .items = file_items,
       .item_count = G_N_ELEMENTS(file_items)},
      {.label = "Help",
       .items = help_items,
       .item_count = G_N_ELEMENTS(help_items)},
  };
  gtk_box_append(GTK_BOX(root), create_menubar(&(menubar_config){
                                     .sections = sections,
                                     .section_count = G_N_ELEMENTS(sections),
                                     .action_map = G_ACTION_MAP(app),
                                     .layout = MENU_LAYOUT_VERTICAL,
                                     .show_arrow_indicators = true,
                                 }));

  GtkWidget *hbox = create_box(&(box_config){
      .orientation = GTK_ORIENTATION_HORIZONTAL,
      .spacing = 0,
      .homogeneous = false,
      .style = {.hexpand = true,
                .set_hexpand = true,
                .vexpand = true,
                .set_vexpand = true},
  });
  GtkWidget *stack = create_stack(&(stack_config){
      .transition = GTK_STACK_TRANSITION_TYPE_SLIDE_UP_DOWN,
      .duration_ms = 280,
      .vhomogeneous = false,
      .hhomogeneous = false,
      .style = {.hexpand = true,
                .set_hexpand = true,
                .vexpand = true,
                .set_vexpand = true},
  });
  GtkWidget *sidebar = create_stack_sidebar(stack, &(widget_style_config){
                                                       .margin_top = 10,
                                                       .margin_bottom = 10,
                                                       .margin_start = 10,
                                                       .margin_end = 6,
                                                   });

  stack_add_page(stack, make_inputs_page(state), "inputs", "Inputs");
  stack_add_page(stack, make_buttons_page(state), "buttons", "Buttons");
  stack_add_page(stack, make_toggles_page(state), "toggles", "Toggles");
  stack_add_page(stack, make_displays_page(state), "displays", "Displays");
  stack_add_page(stack, make_dialogs_page(state), "dialogs", "Dialogs");
  stack_add_page(stack, make_icons_page(state), "icons", "Icons & Images");
  stack_add_page(stack, make_toolbar_page(state), "toolbar", "Toolbar");

  gtk_box_append(GTK_BOX(hbox), sidebar);
  gtk_box_append(GTK_BOX(hbox), stack);
  gtk_box_append(GTK_BOX(root), hbox);

  gtk_box_append(GTK_BOX(root),
                 create_separator(&(separator_config){
                     .orientation = GTK_ORIENTATION_HORIZONTAL}));
  state->status_label = create_label(&(label_config){
      .text = "Ready",
      .selectable = false,
      .wrap = false,
      .ellipsize = PANGO_ELLIPSIZE_END,
      .max_width_chars = 96,
      .style = {.margin_top = 8,
                .margin_bottom = 8,
                .margin_start = 12,
                .margin_end = 12},
  });
  gtk_box_append(GTK_BOX(root), state->status_label);
  return root;
}

void demo_app_start(GtkApplication *app, gpointer user_data) {
  (void)user_data;
  static bool once = false;
  if (!once) {
    apply_demo_theme();
    once = true;
  }

  GtkWidget *window =
      create_window(app, &(window_config){
                             .title = "GTK UI Wrapper Showcasea",
                              .icon_name = "applications-development-symbolic",
                              .widget_name = "wrapper-window",
                              .background_image_path = "image.png",
                             .default_width = 700,
                             .default_height = 300,
                             .min_width = 400,
                             .min_height = 300,
                             .max_width = 0,
                             .max_height = 0,
                             .resizable = true,
                             .decorated = true,
                             .modal = false,
                             .maximized = false,
                             .present_on_create = false,
                             .style = {0},
                         });

  showcase_state *state = g_new0(showcase_state, 1);

  gtk_window_set_child(GTK_WINDOW(window),
                       build_showcase(app, GTK_WINDOW(window), state));
  gtk_window_present(GTK_WINDOW(window));
}
