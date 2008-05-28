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

struct _CaptureStrategyReadPrivate {
	cam   * cam;
	guchar* pic;
};

enum {
	PROP_0,
	PROP_CAMORAMA
};

#define PRIV(i) (CAPTURE_STRATEGY_READ (i)->_private)

/* GType Implementation */

static void implement_capture_strategy (CaptureStrategyIface* iface);
G_DEFINE_TYPE_WITH_CODE (CaptureStrategyRead, capture_strategy_read, G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (CAMORAMA_TYPE_CAPTURE_STRATEGY,
						implement_capture_strategy));

static void
capture_strategy_read_init (CaptureStrategyRead* self)
{
	PRIV(self) = G_TYPE_INSTANCE_GET_PRIVATE (self,
						  CAMORAMA_TYPE_CAPTURE_STRATEGY_READ,
						  CaptureStrategyReadPrivate);
}

static void
read_constructed (GObject* object)
{
	cam* cam = PRIV(object)->cam;

	if (G_OBJECT_CLASS (capture_strategy_read_parent_class)->constructed) {
		G_OBJECT_CLASS (capture_strategy_read_parent_class)->constructed (object);
	}

	g_return_if_fail (PRIV (object)->cam);

	PRIV(object)->pic = realloc (PRIV(object)->pic,
				     cam->vid_cap.maxwidth * cam->vid_cap.maxheight * 3);
}

static void
read_finalize (GObject* object)
{
	PRIV(object)->cam = NULL;

	free (PRIV(object)->pic);

	G_OBJECT_CLASS (capture_strategy_read_parent_class)->finalize (object);
}

static void
read_set_property (GObject     * object,
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
capture_strategy_read_class_init (CaptureStrategyReadClass* self_class)
{
	GObjectClass* object_class = G_OBJECT_CLASS (self_class);

	object_class->constructed  = read_constructed;
	object_class->finalize     = read_finalize;
	object_class->set_property = read_set_property;

	g_object_class_install_property (object_class, PROP_CAMORAMA,
					 g_param_spec_pointer ("cam", "", "",
							       G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

	g_type_class_add_private (self_class, sizeof (CaptureStrategyReadPrivate));
}

/* Public API */

CaptureStrategy*
capture_strategy_read_new (cam* cam)
{
	return g_object_new (CAMORAMA_TYPE_CAPTURE_STRATEGY_READ,
			     "cam", cam,
			     NULL);
}

/* Capture Strategy Implementation */

/*
 * get image from cam - does all the work ;) 
 */
static gboolean
read_timeout_func (cam* cam)
{
    int i;
    GdkGC *gc;

    read (cam->dev, PRIV(cam->capture)->pic, (cam->x * cam->y * 3));
    frames2++;
    /*
     * update_rec.x = 0;
     * update_rec.y = 0;
     * update_rec.width = cam->x;
     * update_rec.height = cam->y;
     */

	/* reference the frame */
	cam->pic_buf = PRIV(cam->capture)->pic;

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

