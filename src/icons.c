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

#include <gtk/gtk.h>

#include "util.h"

struct icon {
	char *start;
	size_t size;
};

// Define pointers to inline icon data:
#define ICON_DEF(size)						\
	extern char _binary_res_icon_## size ##_png_start[];	\
	extern char _binary_res_icon_## size ##_png_end[];

// Define a structure for each inline icon:
#define ICON(size)						\
	{ _binary_res_icon_## size ##_png_start			\
	, _binary_res_icon_## size ##_png_end - _binary_res_icon_## size ##_png_start \
	}

// Add an icon to the list:
static GList *
list_add (GList *list, struct icon *icon)
{
	GInputStream *stream;
	GdkPixbuf *pixbuf;

	// Create an input stream from inline data:
	stream = g_memory_input_stream_new_from_data(icon->start, icon->size, NULL);

	// Generate a pixbuf from the input stream:
	pixbuf = gdk_pixbuf_new_from_stream(stream, NULL, NULL);

	// Add this pixbuf to the list:
	list = g_list_append(list, pixbuf);

	// Destroy the stream:
	g_object_unref(stream);

	return list;
}

// The "X macro" trick to map an operation to a list of variables:
#define X_MAP	\
	X(16)	\
	X(32)	\
	X(48)	\
	X(64)	\
	X(128)	\

// Load all inline icons, use them as window icons:
void
icons_load (void)
{
	// Define pointers:
	#define X(x) ICON_DEF(x);
	X_MAP
	#undef X

	// Define array of structs:
	#define X(x) ICON(x),
	struct icon icons[] = {
		X_MAP
	};
	#undef X

	GList *list = NULL;

	// Add icons to list:
	FOREACH (icons, i)
		list = list_add(list, i);

	// Set icon list as default:
	gtk_window_set_default_icon_list(list);

	// Free the list and unreference the pixbufs:
	g_list_free_full(list, g_object_unref);
}
