/* this file is part of camorama, a gnome webcam viewer
 *
 * AUTHORS
 *	Greg Jones	     <greg@fixedgear.org>
 *	Bastien Nocera	     <hadess@hadess.net>
 *      Sven Herzberg        <herzi@gnome-de.org>
 *
 * Copyright (C) 2003 Greg Jones
 * Copyright (C) 2003 Bastien Nocera
 * Copyright (C) 2005 Sven Herzberg
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

#ifndef CAMORAMA_FILTER_H
#define CAMORAMA_FILTER_H

#include <glib/gtypes.h>

G_BEGIN_DECLS

void yuv420p_to_rgb	(guchar *image, guchar *temp, gint x, gint y, gint z);
void fix_colour		(guchar *image, gint x, gint y);
void negative		(guchar *image, gint x, gint y, gint z);
void threshold		(guchar *image, gint x, gint y, gint threshold_value);
void threshold_channel	(guchar *image, gint x, gint y, gint threshold_value);
void mirror		(guchar *image, gint x, gint y, gint z);
void wacky		(guchar *image, gint z, gint x, gint y);
void smooth		(guchar *image, gint z, gint x, gint y);

void bw			(guchar *image, gint x, gint y);
void bw2		(guchar *image, gint x, gint y);
void laplace		(guchar *image, gint z, gint x, gint y);
void sobel		(guchar *image, gint x, gint y);

G_END_DECLS

#endif /* !CAMORAMA_FILTER_H */

