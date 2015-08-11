#include <glib.h>

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

// Entry point
int
main (int argc, char *argv[])
{
	gchar *dir = get_initial_dir(argc, argv);

	g_free(dir);

	return 0;
}
