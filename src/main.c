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

#include "filelist.h"
#include "gui.h"

static inline gboolean
is_dir (gchar *const path)
{
	return g_file_test(path, G_FILE_TEST_IS_DIR);
}

// Return the directory to search as an allocated string
static gchar *
get_initial_dir (int argc, char *argv[])
{
	// At least one argument:
	if (argc > 1) {
		gchar *dir;

	 	// If the argument is a directory, return it:
		if (is_dir(argv[1]))
			return g_strdup(argv[1]);

		// Else, get the dirname and try that:
		if (is_dir(dir = g_path_get_dirname(argv[1])))
			return dir;

		g_free(dir);
	}

	// Return the current directory:
	return g_strdup(".");
}

// Return pointer to initial file in list to show
static GList *
get_initial_file (int argc, char *argv[], GList *list)
{
	// At least one argument:
	if (argc > 1) {
		GList *file;

		// If the argument is a directory, return first file:
		if (is_dir(argv[1]))
			return list;

		// Else try to find the basename in the file list:
		if ((file = filelist_find(list, argv[1])))
			return file;
	}

	// Return the first file in the list:
	return list;
}

// Entry point
int
main (int argc, char *argv[])
{
	GList *list, *file;
	gchar *dir;
	int ret = 0;

	// Get initial dir:
	dir = get_initial_dir(argc, argv);

	// Get file list for initial dir:
	if ((list = filelist_create(dir)) == NULL) {
		ret = 1;
		goto out0;
	}

	// Get the initial file within the dir:
	if (!(file = get_initial_file(argc, argv, list))) {
		ret = 1;
		goto out1;
	}

	// Scan to next image:
	if (!(file = filelist_scan_forward(&list, file))
	 && !(file = filelist_scan_backward(&list, g_list_last(list)))) {
		ret = 1;
		goto out1;
	}

	// Initialize GUI layer:
	gui_init(argc, argv);

	// Start the GUI on this file:
	gui_run(&list, file, dir);

out1:	filelist_destroy(list);
out0:	g_free(dir);

	return ret;
}
