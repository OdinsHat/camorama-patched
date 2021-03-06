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

#ifndef CAPTURE_STRATEGY_READ_H
#define CAPTURE_STRATEGY_READ_H

#include "capture-strategy.h"

G_BEGIN_DECLS

typedef struct _CaptureStrategyRead        CaptureStrategyRead;
typedef struct _CaptureStrategyReadPrivate CaptureStrategyReadPrivate;
typedef struct _CaptureStrategyReadClass   CaptureStrategyReadClass;

#define CAMORAMA_TYPE_CAPTURE_STRATEGY_READ (capture_strategy_read_get_type ())
#define CAPTURE_STRATEGY_READ(i)            (G_TYPE_CHECK_INSTANCE_CAST ((i), CAMORAMA_TYPE_CAPTURE_STRATEGY_READ, CaptureStrategyRead))

CaptureStrategy* capture_strategy_read_new (cam* cam);

struct _CaptureStrategyRead {
	GObject                     base_instance;
	CaptureStrategyReadPrivate* _private;
};

struct _CaptureStrategyReadClass {
	GObjectClass                base_class;
};

G_END_DECLS

#endif /* !CAPTURE_STRATEGY_READ_H */
