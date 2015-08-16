#include <gtk/gtk.h>

#include "filedata.h"
#include "filelist.h"
#include "geometry.h"
#include "pixbuf.h"

struct state {
	GList **list;
	GList *file;
	GtkWidget *window;
	GtkWidget *darea;
	struct geometry geometry;
	gboolean panning;
};

// Load image into window
static void
gui_load (struct state *state)
{
	struct filedata *fd = state->file->data;

	geometry_reset(&state->geometry, fd->pixbuf);
	gtk_window_set_title(GTK_WINDOW(state->window), fd->path);
	gtk_window_resize(GTK_WINDOW(state->window),
		state->geometry.window_wd,
		state->geometry.window_ht);
	gtk_widget_queue_draw(state->darea);
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

// Redraw current image
static gboolean
on_draw (GtkWidget *widget, cairo_t *cr, struct state *state)
{
	struct filedata *fd = state->file->data;

	// Dark background:
	cairo_set_source_rgb(cr, 0.05, 0.05, 0.05);
	cairo_paint(cr);

	// Image:
	gdk_cairo_set_source_pixbuf(cr, fd->pixbuf,
		state->geometry.offset_x,
		state->geometry.offset_y);

	cairo_paint(cr);

	return FALSE;
}

// Window was moved or resized
static gboolean
on_configure (GtkWidget *widget, GdkEventConfigure *event, struct state *state)
{
	geometry_window_resized(&state->geometry, event->width, event->height);
	return FALSE;
}

// Mouse button was pressed
static gboolean
on_button_press (GtkWidget *widget, GdkEventButton *event, struct state *state)
{
	if (event->button == 1)
		if (state->panning == FALSE) {
			geometry_pan_start(&state->geometry, event->x, event->y);
			state->panning = TRUE;
		}

	return FALSE;
}

// Mouse button was released
static gboolean
on_button_release (GtkWidget *widget, GdkEventButton *event, struct state *state)
{
	if (event->button == 1)
		state->panning = FALSE;

	return FALSE;
}

// Mouse pointer moved in window
static gboolean
on_motion_notify (GtkWidget *widget, GdkEventMotion *event, struct state *state)
{
	if (state->panning == TRUE) {
		geometry_pan_update(&state->geometry, event->x, event->y);
		gtk_widget_queue_draw(state->darea);
	}

	return FALSE;
}

// GUI entry point
void
gui_run (GList **list, GList *file)
{
	struct {
		const gchar *signal;
		GCallback handler;
		GdkEventMask mask;
	}
	signals[] = {
		{ "destroy",			G_CALLBACK(gtk_main_quit),	0			},
		{ "key-press-event",		G_CALLBACK(on_key_press),	GDK_KEY_PRESS_MASK	},
		{ "scroll-event",		G_CALLBACK(on_scroll),		GDK_SCROLL_MASK		},
		{ "configure-event",		G_CALLBACK(on_configure),	0			},
		{ "button-press-event",		G_CALLBACK(on_button_press),	GDK_BUTTON_PRESS_MASK	},
		{ "button-release-event",	G_CALLBACK(on_button_release),	GDK_BUTTON_RELEASE_MASK	},
		{ "motion-notify-event",	G_CALLBACK(on_motion_notify),	GDK_BUTTON1_MOTION_MASK	},
	};

	struct state state = { list, file };

	// Create new window:
	state.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	// Add drawing area:
	state.darea = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(state.window), state.darea);

	// Connect window signals:
	for (size_t i = 0; i < sizeof(signals) / sizeof(signals[0]); i++) {
		gtk_widget_add_events(state.window, signals[i].mask);
		g_signal_connect(
			state.window,
			signals[i].signal,
			signals[i].handler,
			&state);
	}

	// Connect drawing area signals:
	g_signal_connect(state.darea, "draw", G_CALLBACK(on_draw), &state);

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
