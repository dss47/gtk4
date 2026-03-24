#include "widgets/button.h"

#include <string.h>

static void apply_theme_variant(GtkWidget *button, const char *variant) {
    if (variant == NULL || variant[0] == '\0') {
        return;
    }

    if (strcmp(variant, "primary") == 0) {
        gtk_widget_add_css_class(button, "btn-primary");
    } else if (strcmp(variant, "success") == 0) {
        gtk_widget_add_css_class(button, "btn-success");
    } else if (strcmp(variant, "danger") == 0) {
        gtk_widget_add_css_class(button, "btn-danger");
    }
}

GtkWidget *create_button(const button_config *config) {
    GtkWidget *button;
    const char *label;
    widget_style_config effective_style;

    if (config == NULL) {
        return gtk_button_new();
    }

    label = config->label ? config->label : "";
    button = config->use_underline ? gtk_button_new_with_mnemonic(label) : gtk_button_new_with_label(label);
    gtk_button_set_has_frame(GTK_BUTTON(button), config->has_frame);
    gtk_button_set_can_shrink(GTK_BUTTON(button), config->can_shrink);

    if (config->icon_name && config->icon_name[0] != '\0') {
        GtkWidget *content = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        GtkWidget *icon = gtk_image_new_from_icon_name(config->icon_name);
        GtkWidget *text = config->use_underline ? gtk_label_new_with_mnemonic(label) : gtk_label_new(label);
        if (config->icon_size > 0) {
            gtk_image_set_pixel_size(GTK_IMAGE(icon), config->icon_size);
        }
        gtk_box_append(GTK_BOX(content), icon);
        gtk_box_append(GTK_BOX(content), text);
        gtk_button_set_child(GTK_BUTTON(button), content);
    }

    effective_style = config->style;
    if (!effective_style.set_hexpand) {
        effective_style.set_hexpand = true;
        effective_style.hexpand = false;
    }
    if (!effective_style.set_vexpand) {
        effective_style.set_vexpand = true;
        effective_style.vexpand = false;
    }
    if (!effective_style.set_halign && effective_style.width_request == 0) {
        effective_style.set_halign = true;
        effective_style.halign = GTK_ALIGN_START;
    }
    if (!effective_style.set_valign) {
        effective_style.set_valign = true;
        effective_style.valign = GTK_ALIGN_START;
    }

    apply_theme_variant(button, config->theme_variant);
    apply_widget_style(button, &effective_style);

    if (config->on_clicked) {
        g_signal_connect(button, "clicked", G_CALLBACK(config->on_clicked), config->user_data);
    }

    return button;
}

GtkWidget *create_radio_button(const radio_button_config *config) {
    GtkWidget *button = gtk_check_button_new_with_label(config && config->label ? config->label : "");

    if (config == NULL) {
        return button;
    }

    if (config->group_with != NULL) {
        gtk_check_button_set_group(GTK_CHECK_BUTTON(button), GTK_CHECK_BUTTON(config->group_with));
    }
    gtk_check_button_set_active(GTK_CHECK_BUTTON(button), config->is_active);
    apply_widget_style(button, &config->style);

    if (config->on_toggled) {
        g_signal_connect(button, "toggled", G_CALLBACK(config->on_toggled), config->user_data);
    }

    return button;
}
