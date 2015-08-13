#include <gtk/gtk.h>

// GUI entry point
void
gui_run (GList **list, GList *file)
{
	GtkWidget *window;
	GtkWidget *image;

	// Create new window:
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	// Add GtkImage:
	image = gtk_image_new();
	gtk_container_add(GTK_CONTAINER(window), image);

	// Connect signals:
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	// Show window:
	gtk_widget_show_all(window);

	// Enter main loop:
	gtk_main();
}

// Initialize GTK
void
gui_init (int argc, char *argv[])
{
	gtk_init(&argc, &argv);
}
