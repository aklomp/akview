#include <gdk/gdk.h>

#include "geometry.h"
	
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
