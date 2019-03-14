#ifndef CAMORAMA_V4L_H
#define CAMORAMA_V4L_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include <linux/types.h>
#include <libv4l1-videodev.h>
#include <signal.h>
#include <png.h>
#include <glade/glade.h>
#include <gconf/gconf-client.h>

G_BEGIN_DECLS

typedef struct camera cam;

G_END_DECLS

#include "camorama-filter-chain.h"
#include "capture-strategy.h"

G_BEGIN_DECLS

typedef enum {
	PICMAX = 0,
	PICMIN = 1,
	PICHALF = 2
} CamoImageSize;

enum {
	JPEG = 0,
	PNG = 1,
	PPM = 2
};

struct camera {
	CaptureStrategy* capture;
    int dev;
    int x;
    int y;
    int depth;
    int desk_depth;
    CamoImageSize size;
    int contrast, brightness, colour, hue, wb;
    int frame_number;
    struct video_capability vid_cap;
    struct video_picture vid_pic;
    struct video_window vid_win;
    struct video_mmap vid_map; // FIXME: could be moved into CaptureStrategyMmapPrivate
    char *video_dev;
    unsigned char *image;
    gchar *capturefile, *rcapturefile;
    gchar *pixdir, *rpixdir;
    int savetype, rsavetype;
    gchar *rhost, *rlogin, *rpw;
    gchar *ts_string;
    gchar *date_format;
    gboolean debug, read, hidden;
    gboolean cap, rcap, acap, show_adjustments, show_effects;
    gboolean timestamp, rtimestamp, usedate, usestring;
    gboolean rtimefn, timefn;
	GdkPixmap *pixmap;
	GtkWidget *da, *tray_tooltip, *status;
	unsigned char *pic_buf, *tmp;
    guint timeout_id, idle_id;
    guint32 timeout_interval;
    GConfClient *gc;
    GladeXML *xml;
    GtkStatusIcon *tray_icon;

    CamoramaFilterChain* filter_chain;
};

void camera_cap (cam *);
void get_win_info (cam *);
void set_win_info (cam * cam);
void get_pic_info (cam *);
void set_pic_info (cam *);

G_END_DECLS

#endif /* !CAMORAMA_V4L_H */

