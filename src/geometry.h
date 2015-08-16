struct geometry {
	int pixbuf_wd;
	int pixbuf_ht;
	int window_wd;
	int window_ht;
	int offset_x;
	int offset_y;
	int pan_start_x;
	int pan_start_y;
	int pixbuf_native_wd;
	int pixbuf_native_ht;
};

void geometry_reset (struct geometry *g, GdkPixbuf *pixbuf);
void geometry_window_resized (struct geometry *g, int new_wd, int new_ht);
void geometry_pan_start (struct geometry *g, int x, int y);
void geometry_pan_update (struct geometry *g, int x, int y);
void geometry_zoom (struct geometry *g, gfloat zoom_factor);
