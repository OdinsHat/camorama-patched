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

#ifndef CAPTURE_STRATEGY_MMAP_H
#define CAPTURE_STRATEGY_MMAP_H

#include "capture-strategy.h"

G_BEGIN_DECLS

typedef struct _CaptureStrategyMmap        CaptureStrategyMmap;
typedef struct _CaptureStrategyMmapPrivate CaptureStrategyMmapPrivate;
typedef struct _CaptureStrategyMmapClass   CaptureStrategyMmapClass;

#define CAMORAMA_TYPE_CAPTURE_STRATEGY_MMAP         (capture_strategy_mmap_get_type ())

GType capture_strategy_mmap_get_type (void);

CaptureStrategy* capture_strategy_mmap_new (void);

struct _CaptureStrategyMmap {
	GObject                     base_instance;
	CaptureStrategyMmapPrivate* _private;
};

struct _CaptureStrategyMmapClass {
	GObjectClass                base_class;
};

G_END_DECLS

#endif /* !CAPTURE_STRATEGY_MMAP_H */
