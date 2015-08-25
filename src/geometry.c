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

static void
constrain_offset (int *offs, int winsz, int imgsz)
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
	
void
geometry_reset (struct geometry *g, GdkPixbuf *pixbuf)
{
	g->pixbuf_wd = gdk_pixbuf_get_width(pixbuf);
	g->pixbuf_ht = gdk_pixbuf_get_height(pixbuf);
	g->pixbuf_native_wd = g->pixbuf_wd;
	g->pixbuf_native_ht = g->pixbuf_ht;
	g->window_wd = g->pixbuf_wd;
	g->window_ht = g->pixbuf_ht;
	g->offset_x = 0;
	g->offset_y = 0;
	g->zoom_factor = 1.0f;
	g->rotation = 0;
}

void
geometry_window_resized (struct geometry *g, int new_wd, int new_ht)
{
	// Get image location currently in the window center:
	int center_x = g->window_wd / 2 - g->offset_x;
	int center_y = g->window_ht / 2 - g->offset_y;

	// Store new window dimensions:
	g->window_wd = new_wd;
	g->window_ht = new_ht;

	// Keep the old image center point:
	g->offset_x = g->window_wd / 2 - center_x;
	g->offset_y = g->window_ht / 2 - center_y;

	// Make sure the offset makes sense:
	constrain_offset(&g->offset_x, g->window_wd, g->pixbuf_wd);
	constrain_offset(&g->offset_y, g->window_ht, g->pixbuf_ht);
}

void
geometry_pan_start (struct geometry *g, int x, int y)
{
	// Get coordinates relative to image offset:
	g->pan_start_x = x - g->offset_x;
	g->pan_start_y = y - g->offset_y;
}

void
geometry_pan_update (struct geometry *g, int x, int y)
{
	g->offset_x = x - g->pan_start_x;
	g->offset_y = y - g->pan_start_y;

	constrain_offset(&g->offset_x, g->window_wd, g->pixbuf_wd);
	constrain_offset(&g->offset_y, g->window_ht, g->pixbuf_ht);

	// Deduce start pos from constrained offset,
	// in case we panned to the edge:
	g->pan_start_x = x - g->offset_x;
	g->pan_start_y = y - g->offset_y;
}

#define is_rotated \
	(g->rotation == 90 || g->rotation == 270)

static void
find_processed_size (struct geometry *g)
{
	g->pixbuf_wd = (is_rotated) ? g->pixbuf_native_ht : g->pixbuf_native_wd;
	g->pixbuf_ht = (is_rotated) ? g->pixbuf_native_wd : g->pixbuf_native_ht;

	g->pixbuf_wd *= g->zoom_factor;
	g->pixbuf_ht *= g->zoom_factor;

	g->window_wd = g->pixbuf_wd;
	g->window_ht = g->pixbuf_ht;

	g->offset_x = 0;
	g->offset_y = 0;
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
