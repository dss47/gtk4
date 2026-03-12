#include "common.h"

void apply_widget_style(GtkWidget *widget, const widget_style_config *style) {
    if (widget == NULL || style == NULL) {
        return;
    }

    if (style->css_class && style->css_class[0] != '\0') {
        gtk_widget_add_css_class(widget, style->css_class);
    }
    if (style->css_classes != NULL) {
        for (int i = 0; style->css_classes[i] != NULL; i++) {
            gtk_widget_add_css_class(widget, style->css_classes[i]);
        }
    }

    if (style->width_request != 0 || style->height_request != 0) {
        gtk_widget_set_size_request(widget, style->width_request, style->height_request);
    }

    gtk_widget_set_margin_top(widget, style->margin_top);
    gtk_widget_set_margin_bottom(widget, style->margin_bottom);
    gtk_widget_set_margin_start(widget, style->margin_start);
    gtk_widget_set_margin_end(widget, style->margin_end);

    if (style->set_halign) {
        gtk_widget_set_halign(widget, style->halign);
    }
    if (style->set_valign) {
        gtk_widget_set_valign(widget, style->valign);
    }
    if (style->set_hexpand) {
        gtk_widget_set_hexpand(widget, style->hexpand);
    }
    if (style->set_vexpand) {
        gtk_widget_set_vexpand(widget, style->vexpand);
    }
    if (style->set_sensitive) {
        gtk_widget_set_sensitive(widget, style->sensitive);
    }
    if (style->set_visible) {
        gtk_widget_set_visible(widget, style->visible);
    }

    if (style->tooltip && style->tooltip[0] != '\0') {
        gtk_widget_set_tooltip_text(widget, style->tooltip);
    }
}
