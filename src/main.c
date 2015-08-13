#include <glib.h>

#include "filelist.h"

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
get_initial_file (int argc, char *argv[], gchar *dir, GList *list)
{
	// At least one argument:
	if (argc > 1) {
		GList *file;
		gchar *fullname;
		gchar *basename;

		// If the argument is a directory, return first file:
		if (is_dir(argv[1]))
			return list;

		// Else get the basename and try to find it in the list:
		basename = g_path_get_basename(argv[1]);
		fullname = g_build_filename(dir, basename, NULL);
		if ((file = filelist_find(list, fullname))) {
			g_free(fullname);
			g_free(basename);
			return file;
		}

		g_free(fullname);
		g_free(basename);
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
	if (!(file = get_initial_file(argc, argv, dir, list))) {
		ret = 1;
		goto out1;
	}

out1:	filelist_destroy(list);
out0:	g_free(dir);

	return ret;
}
