#include <gtk/gtk.h>
#include <config.h>
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "filter.h"
#include <gnome.h>
#include <libgnomeui/gnome-about.h>
#include <libgnomeui/gnome-propertybox.h>
#include <libgnomeui/gnome-window-icon.h>
#include <pthread.h>

//g-hoppa rulez!!!

extern GtkWidget *main_window, *prefswindow;
extern state func_state;
extern int frames;
extern int frames2;
extern int seconds;
extern GtkWidget *dentry, *entry2, *string_entry;
extern GtkWidget *host_entry, *directory_entry, *filename_entry, *login_entry, *pw_entry;
int frame;

//pref callbacks

void ts_func(GtkWidget * rb, cam * cam)
{
   GConfClient *client;

   client = gconf_client_get_default();
   cam->timestamp = gtk_toggle_button_get_active((GtkToggleButton *) rb);
   gconf_client_set_bool(cam->gc, KEY4, cam->timestamp, NULL);
}

void customstring_func(GtkWidget * rb, cam * cam)
{
   GConfClient *client;

   client = gconf_client_get_default();
   cam->usestring = gtk_toggle_button_get_active((GtkToggleButton *) rb);
   gconf_client_set_bool(cam->gc, KEY18, cam->usestring, NULL);
	gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "string_entry"), cam->usestring);
}

void drawdate_func(GtkWidget * rb, cam * cam)
{
   GConfClient *client;

   client = gconf_client_get_default();
   cam->usedate = gtk_toggle_button_get_active((GtkToggleButton *) rb);
   gconf_client_set_bool(cam->gc, KEY19, cam->usedate, NULL);
}

void append_func(GtkWidget * rb, cam * cam)
{
   GConfClient *client;

   client = gconf_client_get_default();
   cam->timefn = gtk_toggle_button_get_active((GtkToggleButton *) rb);
   gconf_client_set_bool(cam->gc, KEY14, cam->timefn, NULL);

}

//remote append date?
void rappend_func(GtkWidget * rb, cam * cam)
{
   GConfClient *client;

   client = gconf_client_get_default();
   cam->rtimefn = gtk_toggle_button_get_active((GtkToggleButton *) rb);
   gconf_client_set_bool(cam->gc, KEY15, cam->rtimefn, NULL);

}

//local image selection
void jpg_func(GtkWidget * rb, cam * cam)
{
   GConfClient *client;

   client = gconf_client_get_default();
   cam->savetype = JPEG;
   gconf_client_set_int(cam->gc, KEY3, cam->savetype, NULL);

}

void png_func(GtkWidget * rb, cam * cam)
{
   GConfClient *client;

   client = gconf_client_get_default();
   cam->savetype = PNG;
   gconf_client_set_int(cam->gc, KEY3, cam->savetype, NULL);
}
void ppm_func(GtkWidget * rb, cam * cam)
{
   GConfClient *client;

   client = gconf_client_get_default();
   cam->savetype = PPM;
   gconf_client_set_int(cam->gc, KEY3, cam->savetype, NULL);
}


void set_sensitive(cam * cam)
{
   //gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "dentry"), cam->cap);
   //gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "entry2"), cam->cap);
	gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "table4"), cam->cap);
	
   gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "appendbutton"), cam->cap);
   gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "tsbutton"), cam->cap);
   gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "jpgb"), cam->cap);
   gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "pngb"), cam->cap);
   /*gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "host_entry"), cam->rcap);
   gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "login_entry"), cam->rcap);
   gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "pw_entry"), cam->rcap);
   gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "filename_entry"), cam->rcap);
   gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "directory_entry"), cam->rcap);*/
	gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "table5"), cam->rcap);
   gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "timecb"), cam->rcap);
   gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "tsbutton2"), cam->rcap);
   gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "fjpgb"), cam->rcap);
   gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "fpngb"), cam->rcap);
	gtk_widget_set_sensitive(glade_xml_get_widget(cam->xml, "hbox20"), cam->acap);
	
	}
void cap_func(GtkWidget * rb, cam * cam)
{
   GConfClient *client;

   client = gconf_client_get_default();
   cam->cap = gtk_toggle_button_get_active((GtkToggleButton *) rb);
   gconf_client_set_bool(cam->gc, KEY12, cam->cap, NULL);
	set_sensitive(cam);

}

//do remote capture?
void rcap_func(GtkWidget * rb, cam * cam)
{
   GConfClient *client;

   client = gconf_client_get_default();
   cam->rcap = gtk_toggle_button_get_active((GtkToggleButton *) rb);
   gconf_client_set_bool(cam->gc, KEY13, cam->rcap, NULL);
	set_sensitive(cam);
   
}

//do automatic capture?
void acap_func(GtkWidget * rb, cam * cam)
{
   GConfClient *client;

   client = gconf_client_get_default();
   cam->acap = gtk_toggle_button_get_active((GtkToggleButton *) rb);
   gconf_client_set_bool(cam->gc, KEY20, cam->acap, NULL);
   if(cam->acap == TRUE) {
      cam->timeout_id = gtk_timeout_add(cam->timeout_interval, (GSourceFunc) timeout_capture_func, cam);
   } else {
      gtk_timeout_remove(cam->timeout_id);
   }
   set_sensitive(cam);
}

//change capture interval
void interval_change(GtkWidget * sb, cam * cam)
{
   GConfClient *client;

   client = gconf_client_get_default();
   cam->timeout_interval = gtk_spin_button_get_value((GtkSpinButton *) sb) * 60000;
   gconf_client_set_int(cam->gc, KEY21, cam->timeout_interval, NULL);
   gtk_timeout_remove(cam->timeout_id);
   cam->timeout_id = gtk_timeout_add(cam->timeout_interval, (GSourceFunc) timeout_capture_func, cam);
}

//remote image selection
void rjpg_func(GtkWidget * rb, cam * cam)
{
   GConfClient *client;

   client = gconf_client_get_default();
   cam->rsavetype = JPEG;
   gconf_client_set_int(cam->gc, KEY10, cam->rsavetype, NULL);

}

void rpng_func(GtkWidget * rb, cam * cam)
{
   GConfClient *client;

   client = gconf_client_get_default();
   cam->rsavetype = PNG;
   gconf_client_set_int(cam->gc, KEY10, cam->rsavetype, NULL);
}
void rppm_func(GtkWidget * rb, cam * cam)
{
   GConfClient *client;

   client = gconf_client_get_default();
   cam->rsavetype = PPM;
   gconf_client_set_int(cam->gc, KEY10, cam->rsavetype, NULL);
}

//draw remote ts?
void rts_func(GtkWidget * rb, cam * cam)
{
   GConfClient *client;

   client = gconf_client_get_default();
   cam->rtimestamp = gtk_toggle_button_get_active((GtkToggleButton *) rb);
   gconf_client_set_bool(cam->gc, KEY11, cam->rtimestamp, NULL);

}

void gconf_notify_func(GConfClient * client, guint cnxn_id, GConfEntry * entry, char *str)
{
   GConfValue *value;

   value = gconf_entry_get_value(entry);
   str = malloc(sizeof(char) * (strlen(gconf_value_get_string(value)) + 1));
   strcpy(str, gconf_value_get_string(value));

}

void gconf_notify_func_bool(GConfClient * client, guint cnxn_id, GConfEntry * entry, gboolean val)
{
   GConfValue *value;
   value = gconf_entry_get_value(entry);
   val = gconf_value_get_bool(value);

}

void gconf_notify_func_int(GConfClient * client, guint cnxn_id, GConfEntry * entry, int val)
{
   GConfValue *value;
   value = gconf_entry_get_value(entry);
   val = gconf_value_get_int(value);

}
int delete_event(GtkWidget * widget, gpointer data)
{
   gtk_main_quit();
   return FALSE;
}

//make it (prefs) so!
void prefs_func(GtkWidget * okbutton, cam * cam)
{
   GConfClient *client;

   client = gconf_client_get_default();

   if(gnome_file_entry_get_full_path((GnomeFileEntry *) dentry, TRUE) != NULL) {
      //cam->pixdir =
        // malloc(sizeof(char) * (strlen(gnome_file_entry_get_full_path((GnomeFileEntry *) dentry, FALSE)) + 1));
      strncpy(cam->pixdir, gnome_file_entry_get_full_path((GnomeFileEntry *) dentry, FALSE),255);
      gconf_client_set_string(cam->gc, KEY1, cam->pixdir, NULL);

   } else {
      if(cam->debug == TRUE) {
         fprintf(stderr, "null directory\ndirectory unchanged.");
      }
   }
   //this is stupid, even if the string is empty, it will not return NULL
   if(strlen(gtk_entry_get_text((GtkEntry *) entry2)) > 0) {
      cam->capturefile = malloc(sizeof(char) * (strlen(gtk_entry_get_text((GtkEntry *) entry2)) + 1));
      strcpy(cam->capturefile, gtk_entry_get_text((GtkEntry *) entry2));
      gconf_client_set_string(cam->gc, KEY2, cam->capturefile, NULL);
   }

   if(strlen(gtk_entry_get_text((GtkEntry *) host_entry)) > 0) {
      cam->rhost = malloc(sizeof(char) * (strlen(gtk_entry_get_text((GtkEntry *) host_entry)) + 1));
      strcpy(cam->rhost, gtk_entry_get_text((GtkEntry *) host_entry));
      gconf_client_set_string(cam->gc, KEY5, cam->rhost, NULL);
   }
   if(strlen(gtk_entry_get_text((GtkEntry *) login_entry)) > 0) {
      cam->rlogin = malloc(sizeof(char) * (strlen(gtk_entry_get_text((GtkEntry *) login_entry)) + 1));
      strcpy(cam->rlogin, gtk_entry_get_text((GtkEntry *) login_entry));
      gconf_client_set_string(cam->gc, KEY6, cam->rlogin, NULL);
   }
   if(strlen(gtk_entry_get_text((GtkEntry *) pw_entry)) > 0) {
      cam->rpw = malloc(sizeof(char) * (strlen(gtk_entry_get_text((GtkEntry *) pw_entry)) + 1));
      strcpy(cam->rpw, gtk_entry_get_text((GtkEntry *) pw_entry));
      gconf_client_set_string(cam->gc, KEY7, cam->rpw, NULL);
   }
   if(strlen(gtk_entry_get_text((GtkEntry *) directory_entry)) > 0) {
      cam->rpixdir = malloc(sizeof(char) * (strlen(gtk_entry_get_text((GtkEntry *) directory_entry)) + 1));
      strcpy(cam->rpixdir, gtk_entry_get_text((GtkEntry *) directory_entry));
      gconf_client_set_string(cam->gc, KEY8, cam->rpixdir, NULL);
   }
   if(strlen(gtk_entry_get_text((GtkEntry *) filename_entry)) > 0) {
      cam->rcapturefile = malloc(sizeof(char) * (strlen(gtk_entry_get_text((GtkEntry *) filename_entry)) + 1));
      strcpy(cam->rcapturefile, gtk_entry_get_text((GtkEntry *) filename_entry));
      gconf_client_set_string(cam->gc, KEY9, cam->rcapturefile, NULL);
   }
   if(strlen(gtk_entry_get_text((GtkEntry *) string_entry)) > 0) {
      cam->ts_string = malloc(sizeof(char) * (strlen(gtk_entry_get_text((GtkEntry *) string_entry)) + 1));
      strcpy(cam->ts_string, gtk_entry_get_text((GtkEntry *) string_entry));
      gconf_client_set_string(cam->gc, KEY16, cam->ts_string, NULL);
   }
   if(cam->debug == TRUE) {
      fprintf(stderr, "dir now = %s\nfile now = %s\n", cam->pixdir, cam->capturefile);
   }
   gtk_widget_hide(prefswindow);

}

void on_quit1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
   gtk_main_quit();
}

void on_preferences1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
   gtk_widget_show(prefswindow);
}

void on_show_adjustments1_activate(GtkMenuItem * menuitem, cam *cam){
	
	if(GTK_WIDGET_VISIBLE(glade_xml_get_widget(cam->xml, "table6"))){
		gtk_widget_hide(glade_xml_get_widget(cam->xml, "table6"));
		gtk_window_resize(GTK_WINDOW(glade_xml_get_widget(cam->xml, "window2")),320,240);
		cam->show_adjustments = FALSE;
	   
	}else{
		gtk_widget_show(glade_xml_get_widget(cam->xml, "table6"));
		cam->show_adjustments = TRUE;
	}
	gconf_client_set_bool(cam->gc, KEY22, cam->show_adjustments, NULL);
}
void on_about1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
   GtkWidget *about1;
   const gchar *authors[] = { "greg jones  <greg@fixedgear.org>", "Jens Knutson  <tempest@magusbooks.com>",NULL };
   const gchar *documenters[] = { NULL };
   GdkPixbuf *logo = (GdkPixbuf *) create_pixbuf("camorama.png");
   /* TRANSLATORS: Replace this string with your names, one name per line. */
   gchar *translators = _("translator_credits");

   if(!strcmp(translators, "translator_credits"))
      translators = NULL;

   about1 = gnome_about_new("camorama", VERSION,
                            "(C) 2002 greg jones",
                            "View, alter and save images from a webcam", authors, documenters, translators, logo);
   gtk_widget_show(about1);
}

//get image from cam - does all the work ;)
gint timeout_func(cam * cam)
{
   int i, count = 0;
   GdkGC *gc;

   i = -1;
   while(i < 0) {
      i = ioctl(cam->dev, VIDIOCSYNC, &frame);

      if(i < 0 && errno == EINTR) {

         continue;
      }
      if(i < 0) {
         perror("VIDIOCSYNC");
         exit(-1);
      }
      break;
   }
   count++;
   /* refer the frame */
   cam->pic_buf = cam->pic + cam->vid_buf.offsets[frame];
   if(cam->vid_pic.palette == VIDEO_PALETTE_YUV420P) {
      yuv420p_to_rgb(cam->pic_buf, cam->tmp, cam->x, cam->y, cam->depth);
      cam->pic_buf = cam->tmp;
   }
   if(func_state.fc == TRUE) {
      fix_colour(cam->pic_buf, cam->x, cam->y);
   }

   if(func_state.threshold_channel == TRUE) {
      threshold_channel(cam->pic_buf, cam->x, cam->y, cam->dither);
   }

   if(func_state.threshold == TRUE) {
      threshold(cam->pic_buf, cam->x, cam->y, cam->dither);
   }

   if(func_state.edge3 == TRUE) {
      edge3(cam->pic_buf, cam->depth, cam->x, cam->y);
      //laplace(cam->pic_buf, cam->x, cam->y);
      //sobel(cam->pic_buf, cam->x, cam->y);
   }

   if(func_state.sobel == TRUE) {
      sobel(cam->pic_buf, cam->x, cam->y);
   }

   if(func_state.wacky == TRUE) {
      wacky(cam->pic_buf, cam->depth, cam->x, cam->y);
   }

   if(func_state.negative == TRUE) {
      negative(cam->pic_buf, cam->x, cam->y, cam->depth);
   }

   if(func_state.mirror == TRUE) {
      mirror(cam->pic_buf, cam->x, cam->y, cam->depth);
   }

   if(func_state.colour == TRUE) {
      bw(cam->pic_buf, cam->x, cam->y);
   }

   if(func_state.smooth == TRUE) {
      smooth(cam->pic_buf, cam->depth, cam->x, cam->y);
   }

   gc = gdk_gc_new(cam->pixmap);
   /*gdk_draw_rgb_image(cam->pixmap,
    * main_window->style->fg_gc[GTK_STATE_NORMAL], 0, 0,
    * cam->vid_win.width, cam->vid_win.height, GDK_RGB_DITHER_NORMAL,
    * cam->pic_buf, cam->vid_win.width * cam->depth); */
   gdk_draw_rgb_image(cam->pixmap,
                      gc, 0, 0,
                      cam->vid_win.width, cam->vid_win.height, GDK_RGB_DITHER_NORMAL,
                      cam->pic_buf, cam->vid_win.width * cam->depth);
   //cam->da
   gtk_widget_queue_draw_area(glade_xml_get_widget(cam->xml, "da"), 0, 0, cam->x, cam->y);
   //gtk_widget_queue_draw_area(cam->da, 0, 0, cam->x, cam->y);
   //gtk_widget_queue_draw_area(da, 0, 0, cam->x, cam->y);
   cam->vid_map.frame = frame;
   if(ioctl(cam->dev, VIDIOCMCAPTURE, &cam->vid_map) < 0) {
      perror("-/-VIDIOCMCAPTURE");
      exit(-1);
   }

   /* next frame */
   frame++;

   /* reset to the 1st frame */
   if(frame >= cam->vid_buf.frames) {
      frame = 0;
   }

   frames2++;
   return 1;
}

//calculate and draw fps
gint fps(GtkWidget * sb)
{
   const gchar stat[128];

   seconds++;
   sprintf((char *) stat, "%.2f fps - current     %.2f fps - average",
           (float) frames / (float) (2), (float) frames2 / (float) (seconds * 2));
   frames = 0;
   gnome_appbar_push(GNOME_APPBAR(sb), stat);
   return 1;
}

void on_status_show(GtkWidget * sb, cam * cam)
{

   printf("show statusbar\n");
   cam->status = sb;
}

//init cam
void init_cam(GtkWidget * capture, cam * cam)
{
   cam->pic = mmap(0, cam->vid_buf.size, PROT_READ | PROT_WRITE, MAP_SHARED, cam->dev, 0);

   if((unsigned char *) -1 == (unsigned char *) cam->pic) {
      fprintf(stderr, "mmap() failed \n");
      exit(-1);
   }
   cam->vid_map.height = cam->y;
   cam->vid_map.width = cam->x;
   cam->vid_map.format = cam->vid_pic.palette;
   for(frame = 0; frame < cam->vid_buf.frames; frame++) {
      cam->vid_map.frame = frame;
      if(ioctl(cam->dev, VIDIOCMCAPTURE, &cam->vid_map) < 0) {
         perror("--VIDIOCMCAPTURE");
         exit(-1);
      }
   }
   frame = 0;
}

//capture images - called when user clicks capture button
void capture_func(GtkWidget * capture, cam * cam)
{
   GThread *remote_thread;
   //GThread *local_thread;

   if(cam->cap == TRUE) {
      if(local_save(cam) < 0) {
         fprintf(stderr, "save failed\n");
      }
   }
   if(cam->rcap == TRUE) {
      remote_thread = g_thread_create((GThreadFunc) remote_save, cam, FALSE, NULL);
   }

}

//capture images - called by gtk_timeout() if set
gint timeout_capture_func(cam * cam)
{
   pthread_t mythread;

   if(cam->cap == TRUE) {
      if(local_save(cam) < 0) {
         fprintf(stderr, "save failed\n");
      }
   }
   if(cam->rcap == TRUE) {

      if(pthread_create(&mythread, NULL, (void *) remote_save, cam)) {
         printf("error creating thread.");
         abort();
      }
   }
   return 1;
}

//expose event
gboolean on_drawingarea1_expose_event(GtkWidget * widget, GdkEventExpose * event, cam * cam)
{
   //gtk_widget_set_size_request     (widget, cam->x, cam->y);
   gdk_draw_drawable(widget->window,
                     widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
                     cam->pixmap,
                     event->area.x, event->area.y, event->area.x, event->area.y, event->area.width, event->area.height);

   frames++;
   return FALSE;
}

//filter callbacks
void fix_colour_func(GtkToggleButton * tb, char *data)
{
   func_state.fc = gtk_toggle_button_get_active(tb);
}

void edge_func1(GtkToggleButton * tb, gpointer data)
{
   func_state.wacky = gtk_toggle_button_get_active(tb);
}

void sobel_func(GtkToggleButton * tb, gpointer data)
{
   func_state.sobel = gtk_toggle_button_get_active(tb);
}

void threshold_func1(GtkToggleButton * tb, gpointer data)
{
   func_state.threshold = gtk_toggle_button_get_active(tb);
}

void threshold_ch_func(GtkToggleButton * tb, gpointer data)
{
   func_state.threshold_channel = gtk_toggle_button_get_active(tb);
}

void edge_func3(GtkToggleButton * tb, gpointer data)
{
   func_state.edge3 = gtk_toggle_button_get_active(tb);
}

void negative_func(GtkToggleButton * tb, gpointer data)
{
   func_state.negative = gtk_toggle_button_get_active(tb);
}

void mirror_func(GtkToggleButton * tb, gpointer data)
{
   func_state.mirror = gtk_toggle_button_get_active(tb);
}

void smooth_func(GtkToggleButton * tb, gpointer data)
{
   func_state.smooth = gtk_toggle_button_get_active(tb);
}

void colour_func(GtkToggleButton * tb, gpointer data)
{
   func_state.colour = gtk_toggle_button_get_active(tb);
}

//slider callbacks
void on_scale1_drag_data_received(GtkHScale * sc1, cam * cam)
{
   cam->dither = (int) gtk_range_get_value((GtkRange *) sc1);
}

void contrast_change(GtkHScale * sc1, cam * cam)
{

   cam->vid_pic.contrast = 256 * (int) gtk_range_get_value((GtkRange *) sc1);
   set_pic_info(cam);
}

void brightness_change(GtkHScale * sc1, cam * cam)
{

   cam->vid_pic.brightness = 256 * (int) gtk_range_get_value((GtkRange *) sc1);
   set_pic_info(cam);
}

void colour_change(GtkHScale * sc1, cam * cam)
{

   cam->vid_pic.colour = 256 * (int) gtk_range_get_value((GtkRange *) sc1);
   set_pic_info(cam);
}

void hue_change(GtkHScale * sc1, cam * cam)
{

   cam->vid_pic.hue = 256 * (int) gtk_range_get_value((GtkRange *) sc1);
   set_pic_info(cam);
}

void wb_change(GtkHScale * sc1, cam * cam)
{

   cam->vid_pic.whiteness = 256 * (int) gtk_range_get_value((GtkRange *) sc1);
   set_pic_info(cam);
}

//imaginary help cb
void help_cb(GtkWidget * widget, gpointer data)
{
   GError *error = NULL;

   if(error != NULL) {
      /* FIXME: This is bad :) */
      g_warning("%s\n", error->message);
      g_error_free(error);
   }
}
