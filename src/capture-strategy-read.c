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

/* GType Implementation */

G_DEFINE_TYPE_WITH_CODE (CaptureStrategyRead, capture_strategy_read, G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (CAMORAMA_TYPE_CAPTURE_STRATEGY, NULL));

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

