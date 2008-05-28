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

#include "capture-strategy-read.h"

#include "camorama-globals.h"

/* GType Implementation */

static void implement_capture_strategy (CaptureStrategyIface* iface);
G_DEFINE_TYPE_WITH_CODE (CaptureStrategyRead, capture_strategy_read, G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (CAMORAMA_TYPE_CAPTURE_STRATEGY,
						implement_capture_strategy));

static void
capture_strategy_read_init (CaptureStrategyRead* self)
{}

static void
capture_strategy_read_class_init (CaptureStrategyReadClass* self_class)
{}

/* Public API */

CaptureStrategy*
capture_strategy_read_new (void)
{
	return g_object_new (CAMORAMA_TYPE_CAPTURE_STRATEGY_READ,
			     NULL);
}

/* Capture Strategy Implementation */

/*
 * get image from cam - does all the work ;) 
 */
static
gboolean
read_timeout_func(cam* cam) {
    int i, count = 0;
    GdkGC *gc;

    read (cam->dev, cam->pic, (cam->x * cam->y * 3));
    frames2++;
    /*
     * update_rec.x = 0;
     * update_rec.y = 0;
     * update_rec.width = cam->x;
     * update_rec.height = cam->y; 
     */
    count++;
    /*
     * refer the frame 
     */
    cam->pic_buf = cam->pic;    // + cam->vid_buf.offsets[frame];

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
    return TRUE; /* call this function again */
}

static void
implement_capture_strategy (CaptureStrategyIface* iface)
{
	iface->capture = read_timeout_func;
}

