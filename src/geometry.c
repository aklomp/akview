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
	g->window_wd = g->pixbuf_wd;
	g->window_ht = g->pixbuf_ht;
	g->offset_x = 0;
	g->offset_y = 0;
}

void
geometry_window_resized (struct geometry *g, int new_wd, int new_ht)
{
	g->window_wd = new_wd;
	g->window_ht = new_ht;

	// Center current pixbuf inside window:
	g->offset_x = (g->window_wd - g->pixbuf_wd) / 2;
	g->offset_y = (g->window_ht - g->pixbuf_ht) / 2;
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
