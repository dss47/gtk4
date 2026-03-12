#include <gtk/gtk.h>
#include "database.h"

static void on_app_activate(GtkApplication *app, gpointer user_data) {
    sqlite3 *db = (sqlite3 *)user_data;
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Nippon - Anime Slayer Clone");
    gtk_window_set_default_size(GTK_WINDOW(window), 1280, 720);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    sqlite3 *db = db_init("database/nippon.db");
    if (!db) return 1;

    GtkApplication *app = gtk_application_new("com.nippon.app", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), db);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    sqlite3_close(db);
    g_object_unref(app);
    return status;
}