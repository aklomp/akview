// Copyright 2015, Alfred Klomp <git@alfredklomp.com>
//
// This file is part of akview.
//
// Akview is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Akview is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with akview. If not, see <http://www.gnu.org/licenses/>.

#include <gdk/gdk.h>

#include "geometry.h"

static inline void
constrain_offset_axis (int *offs, int winsz, int imgsz)
{
	// If window is larger than image, always center image:
	if (winsz >= imgsz)
		*offs = (winsz - imgsz) / 2;

	// If image is larger than window, window must be inside image:
	if (winsz < imgsz) {
		if (*offs > 0)
			*offs = 0;

		if (*offs < winsz - imgsz)
			*offs = winsz - imgsz;
	}
}

static void
constrain_offset (struct position *offs, const struct size *win, const struct size *img)
{
	constrain_offset_axis(&offs->x, win->width, img->width);
	constrain_offset_axis(&offs->y, win->height, img->height);
}

void
geometry_reset (struct geometry *g, GdkPixbuf *pixbuf)
{
	g->pixbuf.native.width = gdk_pixbuf_get_width(pixbuf);
	g->pixbuf.native.height = gdk_pixbuf_get_height(pixbuf);
	g->pixbuf.active = g->pixbuf.native;
	g->window = g->pixbuf.active;
	g->offset.x = 0;
	g->offset.y = 0;
	g->zoom_factor = 1.0f;
	g->rotation = 0;
}

void
geometry_window_resized (struct geometry *g, const struct size *size)
{
	// Get image location currently in the window center:
	int center_x = g->window.width / 2 - g->offset.x;
	int center_y = g->window.height / 2 - g->offset.y;

	// Store new window dimensions:
	g->window = *size;

	// Keep the old image center point:
	g->offset.x = g->window.width / 2 - center_x;
	g->offset.y = g->window.height / 2 - center_y;

	// Make sure the offset makes sense:
	constrain_offset(&g->offset, &g->window, &g->pixbuf.active);
}

void
geometry_pan_start (struct geometry *g, const struct position *pos)
{
	// Get coordinates relative to image offset:
	g->pan_start.x = pos->x - g->offset.x;
	g->pan_start.y = pos->y - g->offset.y;
}

void
geometry_pan_update (struct geometry *g, const struct position *pos)
{
	g->offset.x = pos->x - g->pan_start.x;
	g->offset.y = pos->y - g->pan_start.y;

	constrain_offset(&g->offset, &g->window, &g->pixbuf.active);

	// Deduce start pos from constrained offset,
	// in case we panned to the edge:
	g->pan_start.x = pos->x - g->offset.x;
	g->pan_start.y = pos->y - g->offset.y;
}

#define is_rotated \
	(g->rotation == 90 || g->rotation == 270)

static void
find_processed_size (struct geometry *g)
{
	g->pixbuf.active.width  = (is_rotated) ? g->pixbuf.native.height : g->pixbuf.native.width;
	g->pixbuf.active.height = (is_rotated) ? g->pixbuf.native.width  : g->pixbuf.native.height;

	g->pixbuf.active.width  *= g->zoom_factor;
	g->pixbuf.active.height *= g->zoom_factor;

	g->window = g->pixbuf.active;

	g->offset.x = 0;
	g->offset.y = 0;
}

void
geometry_zoom (struct geometry *g, gfloat zoom_factor)
{
	g->zoom_factor = zoom_factor;
	find_processed_size(g);
}

void
geometry_rotate (struct geometry *g, int rotation)
{
	g->rotation = rotation;
	find_processed_size(g);
}
