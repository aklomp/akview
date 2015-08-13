#include <glib.h>
#include <glib/gstdio.h>

// Free list of files
void
filelist_destroy (GList *list)
{
	GList *l, *next;

	for (l = list; l != NULL; l = next) {
		next = l->next;
		g_free(l->data);
		list = g_list_delete_link(list, l);
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
			list = g_list_prepend(list, full);
			continue;
		}
		g_free(full);
	}

	// Close dir:
	g_dir_close(dir);

	// Return sorted contents:
	return g_list_sort(list, (GCompareFunc) g_strcmp0);
}

// Find filename in list of files, or NULL
GList *
filelist_find (GList *list, gchar *file)
{
	return g_list_find_custom(list, file, (GCompareFunc) g_strcmp0);
}
