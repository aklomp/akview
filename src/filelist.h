GList *filelist_create (gchar *path);
void filelist_destroy (GList *list);
GList *filelist_find (GList *list, gchar *file);
GList *filelist_scan_forward (GList **list, GList *cur);
GList *filelist_scan_backward (GList **list, GList *cur);
