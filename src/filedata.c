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

#include <glib.h>
#include <gdk/gdk.h>

#include "filedata.h"

// Create new filedata struct
struct filedata *
filedata_create (gchar *path)
{
	struct filedata *fd;

	fd = g_malloc(sizeof *fd);

	fd->path = path;
	fd->basename = g_path_get_basename(path);
	fd->collate_key = g_utf8_collate_key_for_filename(fd->basename, -1);
	fd->pixbuf = NULL;
	fd->zoom_factor = 1.0f;
	fd->rotation = 0;

	return fd;
}

// Destroy filedata struct
void
filedata_destroy (struct filedata *fd)
{
	g_free(fd->path);

	if (fd->pixbuf)
		g_object_unref(fd->pixbuf);

	g_free(fd->collate_key);
	g_free(fd->basename);

	g_free(fd);
}

// Compare two filedata elements by path
gint
filedata_compare (gconstpointer a, gconstpointer b)
{
	const struct filedata *x = a;
	const struct filedata *y = b;

	return g_strcmp0(x->collate_key, y->collate_key);
}
