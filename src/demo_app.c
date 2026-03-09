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
#include "handlers.c"
#include "pages/pages.h"

#include <string.h>


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
       .item_count = G_N_ELEMENTS(file_items),
},
      {.label = "Help",
       .items = help_items,
       .item_count = G_N_ELEMENTS(help_items)},
  };
  container_add(root, create_menubar(&(menubar_config){
                                     .sections = sections,
                                     .section_count = G_N_ELEMENTS(sections),
                                     .action_map = G_ACTION_MAP(app),
                                     .layout = MENU_LAYOUT_VERTICAL,
                                     .show_arrow_indicators = true,
                                 }), 0, 0, 0, 0);

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

  container_add(hbox, sidebar, 0, 0, 0, 0);
  container_add(hbox, stack, 0, 0, 0, 0);
  container_add(root, hbox, 0, 0, 0, 0);

  container_add(root,
                 create_separator(&(separator_config){
                     .orientation = GTK_ORIENTATION_HORIZONTAL}), 0, 0, 0, 0);
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
  container_add(root, state->status_label, 0, 0, 0, 0);
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

  container_add(window,
                       build_showcase(app, GTK_WINDOW(window), state),
                       0, 0, 0, 0);
  gtk_window_present(GTK_WINDOW(window));
}
