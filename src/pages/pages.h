#ifndef PAGES_H
#define PAGES_H

#include <gtk/gtk.h>
#include "demo_app.h"


GtkWidget *make_icons_page(showcase_state *state);
GtkWidget *make_toolbar_page(showcase_state *state);
GtkWidget *make_inputs_page(showcase_state *state);
GtkWidget *make_buttons_page(showcase_state *state);
GtkWidget *make_toggles_page(showcase_state *state);
GtkWidget *make_displays_page(showcase_state *state);
GtkWidget *make_dialogs_page(showcase_state *state);

#endif
