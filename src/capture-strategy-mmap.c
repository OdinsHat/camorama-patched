/* This file is part of camorama
 *
 * AUTHORS
 *     Sven Herzberg  <sven@imendio.com>
 *
 * Copyright (C) 2008  Sven Herzberg
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include "capture-strategy-mmap.h"

#include <errno.h>
#include <glib/gi18n.h>
#include "camorama-globals.h"

struct _CaptureStrategyMmapPrivate {
	// FIXME: get rid of this
	cam   * cam;
	guchar* pic;
};

#define PRIV(i) (CAPTURE_STRATEGY_MMAP(i)->_private)

enum {
	PROP_0,
	PROP_CAMORAMA
};

/* GType Implementation */

static void implement_capture_strategy (CaptureStrategyIface* iface);
G_DEFINE_TYPE_WITH_CODE (CaptureStrategyMmap, capture_strategy_mmap, G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (CAMORAMA_TYPE_CAPTURE_STRATEGY,
						implement_capture_strategy));

static void
capture_strategy_mmap_init (CaptureStrategyMmap* self)
{
	PRIV(self) = G_TYPE_INSTANCE_GET_PRIVATE (self,
						  CAMORAMA_TYPE_CAPTURE_STRATEGY_MMAP,
						  CaptureStrategyMmapPrivate);
}

static void
mmap_constructed (GObject* object)
{
	cam* cam = PRIV(object)->cam;

	if (G_OBJECT_CLASS (capture_strategy_mmap_parent_class)->constructed) {
		G_OBJECT_CLASS (capture_strategy_mmap_parent_class)->constructed (object);
	}

	g_return_if_fail (PRIV (object)->cam);

	/* set the buffer size */
	set_buffer (PRIV(object)->cam);

	PRIV(object)->pic = mmap (0, cam->vid_buf.size, PROT_READ | PROT_WRITE,
				  MAP_SHARED, cam->dev, 0);

    if ((unsigned char *) -1 == (unsigned char *) PRIV(object)->pic) {
        if (cam->debug == TRUE) {
            fprintf (stderr, "Unable to capture image (mmap).\n");
        }
        error_dialog (_("Unable to capture image."));
        exit (-1);
    }
    cam->vid_map.height = cam->y;
    cam->vid_map.width = cam->x;
    cam->vid_map.format = cam->vid_pic.palette;
    for (frame = 0; frame < cam->vid_buf.frames; frame++) {
        cam->vid_map.frame = frame;
        if (ioctl (cam->dev, VIDIOCMCAPTURE, &cam->vid_map) < 0) {
            if (cam->debug == TRUE) {
                fprintf (stderr,
                         "Unable to capture image (VIDIOCMCAPTURE).\n");
            }
            error_dialog (_("Unable to capture image."));
            exit (-1);
        }
    }
    frame = 0;
}

static void
mmap_finalize (GObject* object)
{
	munmap (PRIV (object)->pic, PRIV(object)->cam->vid_buf.size);
	PRIV (object)->cam = NULL;

	G_OBJECT_CLASS (capture_strategy_mmap_parent_class)->finalize (object);
}

static void
mmap_set_property (GObject     * object,
		   guint         prop_id,
		   GValue const* value,
		   GParamSpec  * pspec)
{
	switch (prop_id) {
	case PROP_CAMORAMA:
		PRIV (object)->cam = g_value_get_pointer (value);
		g_object_notify (object, "cam");
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
capture_strategy_mmap_class_init (CaptureStrategyMmapClass* self_class)
{
	GObjectClass* object_class = G_OBJECT_CLASS (self_class);

	object_class->constructed  = mmap_constructed;
	object_class->finalize     = mmap_finalize;
	object_class->set_property = mmap_set_property;

	g_object_class_install_property (object_class, PROP_CAMORAMA,
					 g_param_spec_pointer ("cam", "", "",
							       G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

	g_type_class_add_private (self_class, sizeof (CaptureStrategyMmapPrivate));
}

/* Public API */

CaptureStrategy*
capture_strategy_mmap_new (cam* cam)
{
	return g_object_new (CAMORAMA_TYPE_CAPTURE_STRATEGY_MMAP,
			     "cam", cam,
			     NULL);
}

/* Capture Strategy Implementation */

static
gint
timeout_func (cam * cam)
{
    int i;
    GdkGC *gc;

    i = -1;
    while (i < 0) {
        i = ioctl (cam->dev, VIDIOCSYNC, &frame);

        if (i < 0 && errno == EINTR) {
            if (cam->debug == TRUE) {
                printf ("i = %d\n", i);
            }
            continue;
        }
        if (i < 0) {
            if (cam->debug == TRUE) {
                fprintf (stderr, "Unable to capture image (VIDIOCSYNC)\n");
            }
            error_dialog (_("Unable to capture image."));
            exit (-1);
        }
        break;
    }

	/* reference the frame */
	cam->pic_buf = PRIV(cam->capture)->pic + cam->vid_buf.offsets[frame];
	if (cam->vid_pic.palette == VIDEO_PALETTE_YUV420P) {
		yuv420p_to_rgb (cam->pic_buf, cam->tmp, cam->x, cam->y, cam->depth);
		cam->pic_buf = cam->tmp;
	}

	apply_filters(cam);

    gc = gdk_gc_new (cam->pixmap);

    gdk_draw_rgb_image (cam->pixmap,
                        gc, 0, 0,
                        cam->vid_win.width, cam->vid_win.height,
                        GDK_RGB_DITHER_NORMAL, cam->pic_buf,
                        cam->vid_win.width * cam->depth);

    gtk_widget_queue_draw_area (glade_xml_get_widget (cam->xml, "da"), 0,
                                0, cam->x, cam->y);

    cam->vid_map.frame = frame;
    if (ioctl (cam->dev, VIDIOCMCAPTURE, &cam->vid_map) < 0) {
        if (cam->debug == TRUE) {
            fprintf (stderr, "Unable to capture image (VIDIOCMCAPTURE)\n");
        }
        error_dialog (_("Unable to capture image."));
        exit (-1);
    }

    /*
     * next frame 
     */
    frame++;

    /*
     * reset to the 1st frame 
     */
    if (frame >= cam->vid_buf.frames) {
        frame = 0;
    }

    frames2++;
    g_object_unref ((gpointer) gc);
    return TRUE; /* call this function again */
}

static void
implement_capture_strategy (CaptureStrategyIface* iface)
{
	// FIXME: move timeout_func into this file
	iface->capture = timeout_func;
}

