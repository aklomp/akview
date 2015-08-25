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
#include <glib/gstdio.h>
#include <gdk/gdk.h>

#include "filedata.h"
#include "pixbuf.h"

// Destroy one link
static void
link_destroy (GList **list, GList *link)
{
	filedata_destroy(link->data);
	*list = g_list_delete_link(*list, link);
}

// Check if current link is viable, else delete
static GList *
filelist_scan_current (GList **list, GList *cur)
{
	struct filedata *fd = cur->data;

	// Accept if this entry has a pixbuf:
	if (fd->pixbuf)
		return cur;

	// Else, try to create a pixbuf for this entry:
	if (pixbuf_create(fd))
		return cur;

	// Loading pixbuf failed:
	return NULL;
}

// Scan forward for next viable image
GList *
filelist_scan_forward (GList **list, GList *cur)
{
	while (cur) {
		GList *c, *next = cur->next;

		if ((c = filelist_scan_current(list, cur)))
			return c;

		link_destroy(list, cur);
		cur = next;
	}

	return cur;
}

// Scan backwards for next viable image
GList *
filelist_scan_backward (GList **list, GList *cur)
{
	while (cur) {
		GList *c, *prev = cur->prev;

		if ((c = filelist_scan_current(list, cur)))
			return c;

		link_destroy(list, cur);
		cur = prev;
	}

	return cur;
}

// Free list of files
void
filelist_destroy (GList *list)
{
	GList *l, *next;

	for (l = list; l != NULL; l = next) {
		next = l->next;
		link_destroy(&list, l);
	}
}

// Get sorted GList of all files in dir
GList *
filelist_create (gchar *path)
{
	GDir *dir;
	GList *list = NULL;
	GError *error;
	const gchar *name;

	// Open dir:
	if ((dir = g_dir_open(path, 0, &error)) == NULL) {
		g_printf("%s\n", error->message);
		g_error_free(error);
		return NULL;
	}

	// Get contents:
	while ((name = g_dir_read_name(dir))) {
		gchar *full = g_build_filename(path, name, NULL);
		if (g_file_test(full, G_FILE_TEST_IS_REGULAR)) {
			list = g_list_prepend(list, filedata_create(full));
			continue;
		}
		g_free(full);
	}

	// Close dir:
	g_dir_close(dir);

	// Return sorted contents:
	return g_list_sort(list, filedata_compare);
}

// Find filename in list of files, or NULL
GList *
filelist_find (GList *list, gchar *path)
{
	struct filedata *fd;
	GList *file;

	if ((fd = filedata_create(g_strdup(path))) == NULL)
		return NULL;

	file = g_list_find_custom(list, fd, filedata_compare);
	filedata_destroy(fd);
	return file;
}

// Remove filename and attributes from list of files
void
filelist_unlink (GList **list, GList *file)
{
	link_destroy(list, file);
}

// Add file with given name to list of files
void
filelist_link (GList **list, gchar *fullname)
{
	*list = g_list_insert_sorted(
		*list,
		filedata_create(fullname),
		filedata_compare);
}
