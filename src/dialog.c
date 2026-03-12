#include "dialog.h"

#include <string.h>

typedef struct {
    GtkAlertDialog *dialog;
    void (*on_response)(int response_index, gpointer user_data);
    gpointer user_data;
} alert_async_context;

static void on_alert_response(GObject *source_object, GAsyncResult *result, gpointer user_data) {
    (void)source_object;
    alert_async_context *ctx = user_data;
    GError *error = NULL;
    int index = gtk_alert_dialog_choose_finish(ctx->dialog, result, &error);
    if (error != NULL) {
        g_error_free(error);
        index = -1;
    }
    if (ctx->on_response != NULL) {
        ctx->on_response(index, ctx->user_data);
    }
    g_object_unref(ctx->dialog);
    g_free(ctx);
}

void show_alert_dialog(const alert_dialog_config *config) {
    GtkAlertDialog *dialog;
    const char **buttons;

    if (config == NULL || config->message == NULL) {
        return;
    }

    dialog = gtk_alert_dialog_new("%s", config->message);
    if (config->detail != NULL) {
        gtk_alert_dialog_set_detail(dialog, config->detail);
    }
    gtk_alert_dialog_set_modal(dialog, config->modal);

    if (config->button_count > 0 && config->buttons != NULL) {
        buttons = g_new0(const char *, config->button_count + 1);
        for (size_t i = 0; i < config->button_count; i++) {
            buttons[i] = config->buttons[i].label;
        }
        gtk_alert_dialog_set_buttons(dialog, buttons);
        g_free(buttons);
    }

    int default_button = config->default_button;
    int cancel_button = config->cancel_button;

    if (config->buttons != NULL && config->button_count > 0) {
        for (size_t i = 0; i < config->button_count; i++) {
            const char *appearance = config->buttons[i].appearance;
            if (appearance == NULL) {
                continue;
            }
            if (default_button < 0 && strcmp(appearance, "suggested") == 0) {
                default_button = (int)i;
            }
            if (cancel_button < 0 && strcmp(appearance, "cancel") == 0) {
                cancel_button = (int)i;
            }
        }
    }

    if (default_button >= 0) {
        gtk_alert_dialog_set_default_button(dialog, default_button);
    }
    if (cancel_button >= 0) {
        gtk_alert_dialog_set_cancel_button(dialog, cancel_button);
    }

    if (config->on_response != NULL) {
        alert_async_context *ctx = g_new0(alert_async_context, 1);
        ctx->dialog = g_object_ref(dialog);
        ctx->on_response = config->on_response;
        ctx->user_data = config->user_data;
        gtk_alert_dialog_choose(dialog, config->parent, NULL, on_alert_response, ctx);
    } else {
        gtk_alert_dialog_show(dialog, config->parent);
    }

    g_object_unref(dialog);
}
