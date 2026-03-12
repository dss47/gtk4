#include "display.h"

GtkWidget *create_progress_bar(const progress_bar_config *config) {
    GtkWidget *progress = gtk_progress_bar_new();

    if (config == NULL) {
        return progress;
    }

    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), config->fraction);
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progress), config->show_text);
    gtk_progress_bar_set_inverted(GTK_PROGRESS_BAR(progress), config->inverted);
    gtk_orientable_set_orientation(GTK_ORIENTABLE(progress), config->orientation);
    if (config->text != NULL) {
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress), config->text);
    }
    if (config->pulse_once) {
        gtk_progress_bar_pulse(GTK_PROGRESS_BAR(progress));
    }
    apply_widget_style(progress, &config->style);

    return progress;
}

GtkWidget *create_spinner(const spinner_config *config) {
    GtkWidget *spinner = gtk_spinner_new();

    if (config != NULL) {
        if (config->size > 0) {
            gtk_widget_set_size_request(spinner, config->size, config->size);
        }
        if (config->spinning) {
            gtk_spinner_start(GTK_SPINNER(spinner));
        }
        apply_widget_style(spinner, &config->style);
    }

    return spinner;
}

GtkWidget *create_label(const label_config *config) {
    GtkWidget *label = gtk_label_new(config && config->text ? config->text : "");

    if (config == NULL) {
        return label;
    }

    gtk_label_set_selectable(GTK_LABEL(label), config->selectable);
    gtk_label_set_wrap(GTK_LABEL(label), config->wrap);
    gtk_label_set_ellipsize(GTK_LABEL(label), config->ellipsize);
    if (config->max_width_chars > 0) {
        gtk_label_set_max_width_chars(GTK_LABEL(label), config->max_width_chars);
    }
    apply_widget_style(label, &config->style);

    return label;
}

GtkWidget *create_image(const image_config *config) {
    GtkWidget *widget;

    if (config == NULL) {
        return gtk_image_new();
    }

    if (config->file_path != NULL && config->file_path[0] != '\0') {
        GtkWidget *pic = gtk_picture_new_for_filename(config->file_path);
        gtk_picture_set_can_shrink(GTK_PICTURE(pic), TRUE);
        gtk_picture_set_content_fit(
            GTK_PICTURE(pic),
            config->keep_aspect ? GTK_CONTENT_FIT_CONTAIN : GTK_CONTENT_FIT_FILL
        );
        widget = pic;
    } else if (config->icon_name != NULL && config->icon_name[0] != '\0') {
        GtkWidget *img = gtk_image_new_from_icon_name(config->icon_name);
        if (config->pixel_size > 0) {
            gtk_image_set_pixel_size(GTK_IMAGE(img), config->pixel_size);
        }
        widget = img;
    } else {
        widget = gtk_image_new();
    }

    apply_widget_style(widget, &config->style);
    return widget;
}
