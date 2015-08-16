GList *filelist_create (gchar *path);
void filelist_destroy (GList *list);
GList *filelist_find (GList *list, gchar *basename);
GList *filelist_scan_forward (GList **list, GList *cur);
GList *filelist_scan_backward (GList **list, GList *cur);
void filelist_unlink (GList **list, GList *file);
void filelist_link (GList **list, gchar *path);
