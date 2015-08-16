struct geometry {
	int pixbuf_wd;
	int pixbuf_ht;
	int window_wd;
	int window_ht;
	int offset_x;
	int offset_y;
};

void geometry_reset (struct geometry *g, GdkPixbuf *pixbuf);
