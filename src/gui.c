#include <gtk/gtk.h>

#include "filedata.h"
#include "filelist.h"
#include "pixbuf.h"

static GtkWidget *window;
static GtkWidget *image;

// Load image into window
static void
gui_load (struct filedata *fd)
{
	gtk_image_set_from_pixbuf(GTK_IMAGE(image), fd->pixbuf);
	gtk_window_set_title(GTK_WINDOW(window), fd->path);
	gtk_window_resize(GTK_WINDOW(window),
		gdk_pixbuf_get_width(fd->pixbuf),
		gdk_pixbuf_get_height(fd->pixbuf));
}

// GUI entry point
void
gui_run (GList **list, GList *file)
{
	// Create new window:
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	// Add GtkImage:
	image = gtk_image_new();
	GdkPixbuf *pixbuf = ((struct filedata *)file->data)->pixbuf;
	gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);

	gtk_container_add(GTK_CONTAINER(window), image);

	// Connect signals:
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	// Show window:
	gtk_widget_show_all(window);

	// Load initial image:
	gui_load(file->data);

	// Enter main loop:
	gtk_main();
}

// Initialize GTK
void
gui_init (int argc, char *argv[])
{
	gtk_init(&argc, &argv);
}
