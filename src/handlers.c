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

void on_menu_about(GSimpleAction *action, GVariant *parameter,
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

void on_menu_quit(GSimpleAction *action, GVariant *parameter,
                         gpointer user_data) {
  (void)action;
  (void)parameter;
  g_application_quit(G_APPLICATION(user_data));
}


static void set_status(showcase_state *state, const char *text) {
  if (state == NULL || state->status_label == NULL ||
      !GTK_IS_LABEL(state->status_label)) {
    return;
  }
  gtk_label_set_text(GTK_LABEL(state->status_label), text ? text : "");
}



void on_entry_changed(GtkEditable *editable, gpointer user_data) {
  showcase_state *state = user_data;
  char msg[96];
  g_snprintf(msg, sizeof(msg), "Input length: %zu",
             strlen(gtk_editable_get_text(editable)));
  set_status(state, msg);
}

void on_role_selected(GtkDropDown *dropdown, GParamSpec *pspec,
                             gpointer user_data) {
  (void)pspec;
  showcase_state *state = user_data;
  char msg[96];
  g_snprintf(msg, sizeof(msg), "Role selected index: %u",
             gtk_drop_down_get_selected(dropdown));
  set_status(state, msg);
}

void on_spin_value_changed(GtkSpinButton *spin, gpointer user_data) {
  showcase_state *state = user_data;
  char msg[96];
  g_snprintf(msg, sizeof(msg), "Spin value: %.2f",
             gtk_spin_button_get_value(spin));
  set_status(state, msg);
}

void on_button_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  set_status((showcase_state *)user_data, "Primary action clicked");
}

void on_checkbox_toggled(GtkCheckButton *button, gpointer user_data) {
  showcase_state *state = user_data;
  set_status(state, gtk_check_button_get_active(button) ? "Checkbox enabled"
                                                        : "Checkbox disabled");
}

void on_switch_changed(GtkSwitch *sw, GParamSpec *pspec,
                              gpointer user_data) {
  (void)pspec;
  showcase_state *state = user_data;
  set_status(state,
             gtk_switch_get_active(sw) ? "Switch active" : "Switch inactive");
}

void on_day_selected(GtkCalendar *calendar, gpointer user_data) {
  showcase_state *state = user_data;
  GDateTime *d = gtk_calendar_get_date(calendar);
  char *s = g_date_time_format(d, "%Y-%m-%d");
  char msg[128];
  g_snprintf(msg, sizeof(msg), "Date selected: %s", s);
  set_status(state, msg);
  g_free(s);
  g_date_time_unref(d);
}

void on_alert_result(int response_index, gpointer user_data) {
  showcase_state *state = user_data;
  char msg[96];
  g_snprintf(msg, sizeof(msg), "Dialog response index: %d", response_index);
  set_status(state, msg);
}

void on_show_dialog(GtkButton *button, gpointer user_data) {
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

void on_inc_progress(GtkButton *button, gpointer user_data) {
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

void on_toggle_spinner(GtkButton *button, gpointer user_data) {
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

void on_icon_btn_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  showcase_state *state = user_data;
  state->icon_counter++;
  char msg[96];
  g_snprintf(msg, sizeof(msg), "Icon button clicked %d time(s)", state->icon_counter);
  set_status(state, msg);
}

void on_star_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  set_status((showcase_state *)user_data, "Starred!");
}

void on_copy_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  set_status((showcase_state *)user_data, "Copied to clipboard");
}

void on_search_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  set_status((showcase_state *)user_data, "Search activated");
}

void on_home_clicked(GtkButton *button, gpointer user_data) {
  (void)button;
  set_status((showcase_state *)user_data, "Navigated home");
}

void on_zoom_in(GtkButton *button, gpointer user_data) {
  (void)button;
  showcase_state *state = user_data;
  if (state == NULL || state->icon_preview == NULL ||
      !GTK_IS_IMAGE(state->icon_preview)) {
    return;
  }
  int size = gtk_image_get_pixel_size(GTK_IMAGE(state->icon_preview));
  if (size < 128) {
    size += 8;
    gtk_image_set_pixel_size(GTK_IMAGE(state->icon_preview), size);
  }
  char msg[64];
  g_snprintf(msg, sizeof(msg), "%d px", size);
  if (state->icon_size_label && GTK_IS_LABEL(state->icon_size_label)) {
    gtk_label_set_text(GTK_LABEL(state->icon_size_label), msg);
  }
  set_status(state, "Zoomed in");
}

void on_zoom_out(GtkButton *button, gpointer user_data) {
  (void)button;
  showcase_state *state = user_data;
  if (state == NULL || state->icon_preview == NULL ||
      !GTK_IS_IMAGE(state->icon_preview)) {
    return;
  }
  int size = gtk_image_get_pixel_size(GTK_IMAGE(state->icon_preview));
  if (size > 16) {
    size -= 8;
    gtk_image_set_pixel_size(GTK_IMAGE(state->icon_preview), size);
  }
  char msg[64];
  g_snprintf(msg, sizeof(msg), "%d px", size);
  if (state->icon_size_label && GTK_IS_LABEL(state->icon_size_label)) {
    gtk_label_set_text(GTK_LABEL(state->icon_size_label), msg);
  }
  set_status(state, "Zoomed out");
}

void on_icon_selected(GtkDropDown *dropdown, GParamSpec *pspec,
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