#include <gtk/gtk.h>

#include "filedata.h"
#include "filelist.h"
#include "pixbuf.h"

struct state {
	GList **list;
	GList *file;
	GtkWidget *window;
	GtkWidget *image;
};

// Load image into window
static void
gui_load (struct state *state)
{
	struct filedata *fd = state->file->data;

	gtk_image_set_from_pixbuf(GTK_IMAGE(state->image), fd->pixbuf);
	gtk_window_set_title(GTK_WINDOW(state->window), fd->path);
	gtk_window_resize(GTK_WINDOW(state->window),
		gdk_pixbuf_get_width(fd->pixbuf),
		gdk_pixbuf_get_height(fd->pixbuf));
}

// Move pixbuf to previous picture
static void
move_to_prev (struct state *state)
{
	GList *file;

	// Try to find previous file in list; if this fails, keep current:
	if (!(file = filelist_scan_backward(state->list, state->file->prev)))
		return;

	// Else move to the new file:
	state->file = file;
	gui_load(state);
}

// Move pixbuf to next picture
static void
move_to_next (struct state *state)
{
	GList *file;

	// Try to find next file in list; if this fails, keep current:
	if (!(file = filelist_scan_forward(state->list, state->file->next)))
		return;

	// Else move to the new file:
	state->file = file;
	gui_load(state);
}

// Move pixbuf to first picture
static void
move_to_first (struct state *state)
{
	GList *file;

	// Try to find first file: if this fails, keep current:
	if (!(file = filelist_scan_forward(state->list, g_list_first(*state->list))))
		return;

	// If this is the file we're already on, keep current:
	if (file == state->file)
		return;

	// Else move to the new file:
	state->file = file;
	gui_load(state);
}

// Move pixbuf to last picture
static void
move_to_last (struct state *state)
{
	GList *file;

	// Try to find last file; if this fails, keep current:
	if (!(file = filelist_scan_backward(state->list, g_list_last(*state->list))))
		return;

	// If this is the file we're already on, keep current:
	if (file == state->file)
		return;

	// Else move to the new file:
	state->file = file;
	gui_load(state);
}

// Key was pressed
static gboolean
on_key_press (GtkWidget *widget, GdkEventKey *event, struct state *state)
{
	switch (event->keyval)
	{
	case GDK_KEY_Page_Up:
		move_to_prev(state);
		break;

	case GDK_KEY_Page_Down:
		move_to_next(state);
		break;

	case GDK_KEY_Home:
		move_to_first(state);
		break;

	case GDK_KEY_End:
		move_to_last(state);
		break;

	default:
		break;
	}

	return FALSE;
}

// Mouse wheel was scrolled
static gboolean
on_scroll (GtkWidget* widget, GdkEventScroll *event, struct state *state)
{
	switch (event->direction)
	{
	case GDK_SCROLL_UP:
		move_to_prev(state);
		break;

	case GDK_SCROLL_DOWN:
		move_to_next(state);
		break;

	default:
		break;
	}

	return FALSE;
}

// GUI entry point
void
gui_run (GList **list, GList *file)
{
	struct state state = { list, file };

	// Create new window:
	state.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	// Add GtkImage:
	state.image = gtk_image_new();
	gtk_container_add(GTK_CONTAINER(state.window), state.image);

	// Connect signals:
	gtk_widget_set_events(state.window, GDK_KEY_PRESS_MASK | GDK_SCROLL_MASK);
	g_signal_connect(state.window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(state.window, "key-press-event", G_CALLBACK(on_key_press), &state);
	g_signal_connect(state.window, "scroll-event", G_CALLBACK(on_scroll), &state);

	// Show window:
	gtk_widget_show_all(state.window);

	// Load initial image:
	gui_load(&state);

	// Enter main loop:
	gtk_main();
}

// Initialize GTK
void
gui_init (int argc, char *argv[])
{
	gtk_init(&argc, &argv);
}
