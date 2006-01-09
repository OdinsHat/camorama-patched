/* This file is part of ...
 *
 * AUTHORS
 *     Sven Herzberg  <herzi@gnome-de.org>
 *
 * Copyright (C) 2003  Greg Jones
 * Copyright (C) 2006  Sven Herzberg <herzi@gnome-de.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include "camorama-window.h"

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <glib/gi18n.h>
#include <libgnomeui/gnome-file-entry.h>
#include "callbacks.h"
#include "camorama-filter-chain.h"
#include "camorama-globals.h"
#include "camorama-stock-items.h"
#include "filter.h"

static GQuark menu_item_filter_type = 0;

static void
add_filter_clicked(GtkMenuItem* menuitem, CamoramaFilterChain* chain) {
	GType filter_type = GPOINTER_TO_INT(g_object_get_qdata(G_OBJECT(menuitem), menu_item_filter_type));
	camorama_filter_chain_append(chain, filter_type);
}

static void
menu_position_func(GtkMenu* menu, gint* x, gint* y, gboolean *push_in, gpointer user_data) {
	if(user_data) {
		GdkEventButton* ev = (GdkEventButton*)user_data;
		*x = ev->x;
		*y = ev->y;
	} else {
		// find the selected row and open the popup there
	}
}

static void
show_popup(cam* cam, GtkTreeView* treeview, GdkEventButton* ev) {
	// FIXME: build the menu with the ui manager
	GtkMenu* menu = GTK_MENU(gtk_menu_new());
	GtkWidget* add = gtk_menu_item_new_with_mnemonic(_("_Add Filter"));
	GtkWidget* add_filters = gtk_menu_new();
	GType* filters;
	guint n_filters, i;
	GtkTreeModel* model = gtk_tree_view_get_model(treeview);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(add), add_filters);
	gtk_container_add(GTK_CONTAINER(menu), add);

	filters = g_type_children(CAMORAMA_TYPE_FILTER, &n_filters);
	for(i = 0; i < n_filters; i++) {
		CamoramaFilterClass* filter_class = g_type_class_ref(filters[i]);
		gchar const* filter_name = filter_class->name;
		GtkWidget* menuitem;

		if(!filter_name) {
			filter_name = g_type_name(filters[i]);
		}

		menuitem = gtk_menu_item_new_with_label(filter_name);
		g_object_set_qdata(G_OBJECT(menuitem), menu_item_filter_type, GINT_TO_POINTER(filters[i]));
		g_signal_connect(menuitem, "activate",
				 G_CALLBACK(add_filter_clicked), model);
		gtk_container_add(GTK_CONTAINER(add_filters), menuitem);
		g_type_class_unref(filter_class);
	}
	g_free(filters);
	
	gtk_widget_show_all(GTK_WIDGET(menu));
	gtk_menu_popup(menu,
		       NULL, NULL,
		       menu_position_func, ev,
		       ev ? ev->button : 0,
		       ev ? ev->time : gtk_get_current_event_time());
}

static void
treeview_popup_menu_cb(cam* cam, GtkTreeView* treeview) {
	show_popup(cam, treeview, NULL);
}

static gboolean
treeview_clicked_cb(cam* cam, GdkEventButton* ev, GtkTreeView* treeview) {
	gboolean retval = GTK_WIDGET_GET_CLASS(treeview)->button_press_event(GTK_WIDGET(treeview), ev);

	if(ev->button == 3) {
		show_popup(cam, treeview, NULL);
		retval = TRUE;
	}

	return retval;
}

static gint
tray_clicked_callback (GtkWidget * widget, GdkEventButton * event, cam * cam){
    GdkEventButton *event_button = NULL;

    if (event->type == GDK_BUTTON_PRESS) {

        event_button = (GdkEventButton *) event;

        //change to switch
        if (event_button->button == 1) {
            if (GTK_WIDGET_VISIBLE
                (glade_xml_get_widget (cam->xml, "main_window"))) {
                cam->hidden = TRUE;
                gtk_idle_remove (cam->idle_id);
                gtk_widget_hide (glade_xml_get_widget
                                 (cam->xml, "main_window"));

            } else {
                cam->idle_id =
                    gtk_idle_add ((GSourceFunc) pt2Function, (gpointer) cam);
                gtk_widget_show (glade_xml_get_widget
                                 (cam->xml, "main_window"));
                cam->hidden = FALSE;

            }
            return TRUE;
        } else if (event_button->button == 3) {

            //gw = MyApp->GetMainWindow ();

            //gnomemeeting_component_view (NULL, (gpointer) gw->ldap_window);

            return TRUE;
        }
    }

    return FALSE;
}

void
load_interface(cam* cam) {
    gchar *title;
    GdkPixbuf *logo = NULL;
    GtkWidget *eventbox = NULL, *image = NULL;
    GtkTreeView* treeview = GTK_TREE_VIEW(glade_xml_get_widget(cam->xml, "treeview_effects"));

    menu_item_filter_type = g_quark_from_static_string("camorama-menu-item-filter-type");

    /* set up the tree view */
    gtk_tree_view_insert_column_with_attributes(treeview, -1,
		    				_("Effects"), gtk_cell_renderer_text_new(),
						"text", CAMORAMA_FILTER_CHAIN_COL_NAME,
						NULL);
    cam->filter_chain = camorama_filter_chain_new();
    gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(cam->filter_chain));
    g_object_unref(cam->filter_chain);
    g_signal_connect_swapped(treeview, "button-press-event",
		    	     G_CALLBACK(treeview_clicked_cb), cam);
    g_signal_connect_swapped(treeview, "popup-menu",
		    	     G_CALLBACK(treeview_popup_menu_cb), cam);

    cam->tooltips = gtk_tooltips_new();
    logo = gtk_icon_theme_load_icon(gtk_icon_theme_get_for_screen(gtk_widget_get_screen(glade_xml_get_widget(cam->xml, "main_window"))), CAMORAMA_STOCK_WEBCAM, 24, 0, NULL);
    gtk_window_set_default_icon(logo);
    logo = (GdkPixbuf *) create_pixbuf (DATADIR "/pixmaps/camorama.png");
    if (logo == NULL) {
        printf ("\n\nLOGO NO GO\n\n");
    }

    image = gtk_image_new_from_stock (CAMORAMA_STOCK_WEBCAM, GTK_ICON_SIZE_MENU);

    if (cam->show_adjustments == FALSE) {
        gtk_widget_hide (glade_xml_get_widget
                         (cam->xml, "adjustments_table"));

        gtk_window_resize (GTK_WINDOW
                           (glade_xml_get_widget
                            (cam->xml, "main_window")), 320, 240);
    }
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(glade_xml_get_widget(cam->xml, "showadjustment_item")), cam->show_adjustments);
    if (cam->show_effects == FALSE) {
        gtk_widget_hide (glade_xml_get_widget (cam->xml, "scrolledwindow_effects"));
        gtk_window_resize (GTK_WINDOW
                           (glade_xml_get_widget
                            (cam->xml, "main_window")), 320, 240);
    }
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(glade_xml_get_widget(cam->xml, "show_effects")), cam->show_effects);
    eventbox = gtk_event_box_new();

    gtk_widget_show (image);

    gtk_widget_show (eventbox);

    cam->tray_icon = egg_tray_icon_new ("Our cool tray icon");
    update_tooltip (cam);
    /* add the status to the plug */
    g_object_set_data (G_OBJECT (cam->tray_icon), "image", image);
    g_object_set_data (G_OBJECT (cam->tray_icon), "available",
                       GINT_TO_POINTER (1));
    g_object_set_data (G_OBJECT (cam->tray_icon), "embedded",
                       GINT_TO_POINTER (0));
    gtk_container_add (GTK_CONTAINER (eventbox), image);
    gtk_container_add ((GtkContainer *) cam->tray_icon, eventbox);

    g_signal_connect (G_OBJECT (eventbox), "button_press_event",
                      G_CALLBACK (tray_clicked_callback), cam);

    //button = gtk_button_new_with_label ("This is a cool\ntray icon");
    /*g_signal_connect (button, "clicked",
     * G_CALLBACK (capture_func), cam);
     * gtk_container_add (GTK_CONTAINER (cam->tray_icon), button); */
    gtk_widget_show_all (GTK_WIDGET (cam->tray_icon));

    /* connect the signals in the interface 
     * glade_xml_signal_autoconnect(xml);
     * this won't work, can't pass data to callbacks.  have to do it individually :(*/

    title = g_strdup_printf ("Camorama - %s - %dx%d", cam->vid_cap.name,
                             cam->x, cam->y);
    gtk_window_set_title (GTK_WINDOW
                          (glade_xml_get_widget (cam->xml, "main_window")),
                          title);
    g_free (title);

    gtk_window_set_icon (GTK_WINDOW
                         (glade_xml_get_widget (cam->xml, "main_window")),
                         logo);
    gtk_window_set_icon (GTK_WINDOW
                         (glade_xml_get_widget (cam->xml, "prefswindow")),
                         logo);

    glade_xml_signal_connect_data (cam->xml, "on_show_effects_activate",
                                   G_CALLBACK (on_show_effects_activate),
                                   cam);
    gtk_toggle_button_set_active ((GtkToggleButton *)
                                  glade_xml_get_widget (cam->xml,
                                                        "togglebutton1"),
                                  cam->show_adjustments);
    glade_xml_signal_connect_data (cam->xml,
                                   "on_show_adjustments_activate",
                                   G_CALLBACK
                                   (on_show_adjustments_activate), cam);

    glade_xml_signal_connect_data (cam->xml, "on_large_activate",
                                   G_CALLBACK (on_change_size_activate), cam);
    glade_xml_signal_connect_data (cam->xml, "on_medium_activate",
                                   G_CALLBACK (on_change_size_activate), cam);
    glade_xml_signal_connect_data (cam->xml, "on_small_activate",
                                   G_CALLBACK (on_change_size_activate), cam);

    //glade_xml_signal_connect_data(cam->xml, "capture_func", G_CALLBACK(on_change_size_activate), cam);
    glade_xml_signal_connect_data (cam->xml, "capture_func",
                                   G_CALLBACK (capture_func), cam);
    glade_xml_signal_connect_data (cam->xml, "gtk_main_quit",
                                   G_CALLBACK (delete_event), NULL);

    /* sliders */
    glade_xml_signal_connect_data (cam->xml, "contrast_change",
                                   G_CALLBACK (contrast_change), cam);
    gtk_range_set_value ((GtkRange *)
                         glade_xml_get_widget (cam->xml, "slider2"),
                         (int) (cam->contrast / 256));
    glade_xml_signal_connect_data (cam->xml, "brightness_change",
                                   G_CALLBACK (brightness_change), cam);
    gtk_range_set_value ((GtkRange *)
                         glade_xml_get_widget (cam->xml, "slider3"),
                         (int) (cam->brightness / 256));
    glade_xml_signal_connect_data (cam->xml, "colour_change",
                                   G_CALLBACK (colour_change), cam);
    gtk_range_set_value ((GtkRange *)
                         glade_xml_get_widget (cam->xml, "slider4"),
                         (int) (cam->colour / 256));
    glade_xml_signal_connect_data (cam->xml, "hue_change",
                                   G_CALLBACK (hue_change), cam);
    gtk_range_set_value ((GtkRange *)
                         glade_xml_get_widget (cam->xml, "slider5"),
                         (int) (cam->hue / 256));
    glade_xml_signal_connect_data (cam->xml, "wb_change",
                                   G_CALLBACK (wb_change), cam);
    gtk_range_set_value ((GtkRange *)
                         glade_xml_get_widget (cam->xml, "slider6"),
                         (int) (cam->wb / 256));

    /* buttons */
    glade_xml_signal_connect_data (cam->xml,
                                   "on_drawingarea_expose_event",
                                   G_CALLBACK
                                   (on_drawingarea_expose_event),
                                   (gpointer) cam);
    glade_xml_signal_connect_data (cam->xml, "on_status_show",
                                   G_CALLBACK (on_status_show),
                                   (gpointer) cam);
    glade_xml_signal_connect_data (cam->xml, "on_quit_activate",
                                   G_CALLBACK (on_quit_activate),
                                   (gpointer) cam);
    glade_xml_signal_connect_data (cam->xml, "on_preferences1_activate",
                                   G_CALLBACK (on_preferences1_activate),
                                   (gpointer) cam);
    glade_xml_signal_connect_data (cam->xml, "on_about_activate",
                                   G_CALLBACK (on_about_activate),
                                   (gpointer) cam);

    /* prefs */
    glade_xml_signal_connect_data (cam->xml, "prefs_func",
                                   G_CALLBACK (prefs_func), cam);

    /* general */
    glade_xml_signal_connect_data (cam->xml, "cap_func",
                                   G_CALLBACK (cap_func), cam);

    gtk_toggle_button_set_active ((GtkToggleButton *)
                                  glade_xml_get_widget (cam->xml,
                                                        "captured_cb"),
                                  cam->cap);

    glade_xml_signal_connect_data (cam->xml, "rcap_func",
                                   G_CALLBACK (rcap_func), cam);
    gtk_toggle_button_set_active ((GtkToggleButton *)
                                  glade_xml_get_widget (cam->xml,
                                                        "rcapture"),
                                  cam->rcap);

    glade_xml_signal_connect_data (cam->xml, "acap_func",
                                   G_CALLBACK (acap_func), cam);
    gtk_toggle_button_set_active ((GtkToggleButton *)
                                  glade_xml_get_widget (cam->xml,
                                                        "acapture"),
                                  cam->acap);

    glade_xml_signal_connect_data (cam->xml, "interval_change",
                                   G_CALLBACK (interval_change), cam);

    gtk_spin_button_set_value ((GtkSpinButton *)
                               glade_xml_get_widget (cam->xml,
                                                     "interval_entry"),
                               (cam->timeout_interval / 60000));

    /* local */
    dentry = glade_xml_get_widget (cam->xml, "dentry");
    entry2 = glade_xml_get_widget (cam->xml, "entry2");
    gtk_entry_set_text (GTK_ENTRY
                        (gnome_file_entry_gtk_entry
                         (GNOME_FILE_ENTRY (dentry))), cam->pixdir);

    gtk_entry_set_text (GTK_ENTRY (entry2), cam->capturefile);

    glade_xml_signal_connect_data (cam->xml, "append_func",
                                   G_CALLBACK (append_func), cam);
    gtk_toggle_button_set_active ((GtkToggleButton *)
                                  glade_xml_get_widget (cam->xml,
                                                        "appendbutton"),
                                  cam->timefn);

    glade_xml_signal_connect_data (cam->xml, "jpg_func",
                                   G_CALLBACK (jpg_func), cam);
    if (cam->savetype == JPEG) {
        gtk_toggle_button_set_active ((GtkToggleButton *)
                                      glade_xml_get_widget (cam->xml,
                                                            "jpgb"), TRUE);
    }
    glade_xml_signal_connect_data (cam->xml, "png_func",
                                   G_CALLBACK (png_func), cam);
    if (cam->savetype == PNG) {
        gtk_toggle_button_set_active ((GtkToggleButton *)
                                      glade_xml_get_widget (cam->xml,
                                                            "pngb"), TRUE);
    }

    glade_xml_signal_connect_data (cam->xml, "ts_func",
                                   G_CALLBACK (ts_func), cam);
    gtk_toggle_button_set_active ((GtkToggleButton *)
                                  glade_xml_get_widget (cam->xml,
                                                        "tsbutton"),
                                  cam->timestamp);

    /* remote */
    login_entry = glade_xml_get_widget (cam->xml, "login_entry");
    host_entry = glade_xml_get_widget (cam->xml, "host_entry");
    pw_entry = glade_xml_get_widget (cam->xml, "pw_entry");
    directory_entry = glade_xml_get_widget (cam->xml, "directory_entry");
    filename_entry = glade_xml_get_widget (cam->xml, "filename_entry");
    gtk_entry_set_text (GTK_ENTRY (host_entry), cam->rhost);
    gtk_entry_set_text (GTK_ENTRY (login_entry), cam->rlogin);
    gtk_entry_set_text (GTK_ENTRY (pw_entry), cam->rpw);
    gtk_entry_set_text (GTK_ENTRY (directory_entry), cam->rpixdir);
    gtk_entry_set_text (GTK_ENTRY (filename_entry), cam->rcapturefile);

    glade_xml_signal_connect_data (cam->xml, "rappend_func",
                                   G_CALLBACK (rappend_func), cam);
    gtk_toggle_button_set_active ((GtkToggleButton *)
                                  glade_xml_get_widget (cam->xml,
                                                        "timecb"),
                                  cam->rtimefn);

    glade_xml_signal_connect_data (cam->xml, "rjpg_func",
                                   G_CALLBACK (rjpg_func), cam);
    if (cam->rsavetype == JPEG) {
        gtk_toggle_button_set_active ((GtkToggleButton *)
                                      glade_xml_get_widget (cam->xml,
                                                            "fjpgb"), TRUE);
    }
    glade_xml_signal_connect_data (cam->xml, "rpng_func",
                                   G_CALLBACK (rpng_func), cam);
    if (cam->rsavetype == PNG) {
        gtk_toggle_button_set_active ((GtkToggleButton *)
                                      glade_xml_get_widget (cam->xml,
                                                            "fpngb"), TRUE);
    }

    glade_xml_signal_connect_data (cam->xml, "rts_func",
                                   G_CALLBACK (rts_func), cam);
    gtk_toggle_button_set_active ((GtkToggleButton *)
                                  glade_xml_get_widget (cam->xml,
                                                        "tsbutton2"),
                                  cam->rtimestamp);

    /* timestamp */
    glade_xml_signal_connect_data (cam->xml, "customstring_func",
                                   G_CALLBACK (customstring_func), cam);
    gtk_toggle_button_set_active ((GtkToggleButton *)
                                  glade_xml_get_widget (cam->xml, "cscb"),
                                  cam->usestring);

    string_entry = glade_xml_get_widget (cam->xml, "string_entry");
    gtk_entry_set_text (GTK_ENTRY (string_entry), cam->ts_string);

    glade_xml_signal_connect_data (cam->xml, "drawdate_func",
                                   G_CALLBACK (drawdate_func), cam);
    gtk_toggle_button_set_active ((GtkToggleButton *)
                                  glade_xml_get_widget (cam->xml, "tscb"),
                                  cam->usedate);

    cam->status = glade_xml_get_widget (cam->xml, "status");
    set_sensitive (cam);
    gtk_widget_set_sensitive (glade_xml_get_widget
                              (cam->xml, "string_entry"), cam->usestring);

    gtk_widget_set_size_request (glade_xml_get_widget (cam->xml, "da"),
                                 cam->x, cam->y);

    prefswindow = glade_xml_get_widget (cam->xml, "prefswindow");
}


