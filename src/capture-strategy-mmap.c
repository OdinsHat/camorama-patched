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

/* GType Implementation */

static void implement_capture_strategy (CaptureStrategyIface* iface);
G_DEFINE_TYPE_WITH_CODE (CaptureStrategyMmap, capture_strategy_mmap, G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (CAMORAMA_TYPE_CAPTURE_STRATEGY,
						implement_capture_strategy));

static void
capture_strategy_mmap_init (CaptureStrategyMmap* self)
{}

static void
capture_strategy_mmap_class_init (CaptureStrategyMmapClass* self_class)
{}

/* Public API */

CaptureStrategy*
capture_strategy_mmap_new (void)
{
	return g_object_new (CAMORAMA_TYPE_CAPTURE_STRATEGY_MMAP,
			     NULL);
}

/* Capture Strategy Implementation */

static
gint
timeout_func (cam * cam)
{
    int i, count = 0;
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
    count++;
    /*
     * refer the frame 
     */
    cam->pic_buf = cam->pic + cam->vid_buf.offsets[frame];
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

