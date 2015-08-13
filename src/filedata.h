struct filedata {
	gchar *path;
	gchar *basename;
	gchar *collate_key;
	GdkPixbuf *pixbuf;
};

struct filedata *filedata_create (gchar *path);
void filedata_destroy (struct filedata *fd);
gint filedata_compare (gconstpointer a, gconstpointer b);