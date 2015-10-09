// Copyright 2015, Alfred Klomp <git@alfredklomp.com>
//
// This file is part of akview.
//
// Akview is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Akview is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with akview. If not, see <http://www.gnu.org/licenses/>.

#include <glib.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>

#include "filedata.h"
#include "filelist.h"
#include "geometry.h"
#include "icons.h"
#include "monitor.h"
#include "pixbuf.h"

struct state {
	GList **list;
	GList *file;
	GtkWidget *window;
	GtkWidget *darea;
	struct geometry geometry;
	GdkPixbuf *processed;
	gboolean panning;
	gint screen_wd;
	gint screen_ht;
};

// Zoom factors, powers of two in steps of 0.5:
static const gfloat zoom_table[] = {
	0.125000f,
	0.176777f,
	0.250000f,
	0.353553f,
	0.500000f,
	0.707107f,
	1.000000f,
	1.414214f,
	2.000000f,
	2.828427f,
	4.000000f,
	5.656854f,
	8.000000f,
};

static size_t zoom_table_size = sizeof(zoom_table) / sizeof(zoom_table[0]);

// Set drag cursor
static void
drag_cursor_set (struct state *state)
{
	GdkWindow *window = gtk_widget_get_window(state->window);
	GdkDisplay *display = gdk_window_get_display(window);
	GdkCursor *cursor = gdk_cursor_new_for_display(display, GDK_HAND2);

	gdk_window_set_cursor(window, cursor);
	g_object_unref(cursor);
}

static void
drag_cursor_remove (struct state *state)
{
	gdk_window_set_cursor(gtk_widget_get_window(state->window), NULL);
}

// Reset processed pixbuf
static void
processed_reset (struct state *state)
{
	if (!state->processed)
		return;

	g_object_unref(state->processed);
	state->processed = NULL;
}

// Zoom native pixbuf by factor given in file data
static void
processed_create (struct state *state)
{
	struct filedata *fd = state->file->data;

	// Recalculate layout:
	geometry_rotate(&state->geometry, fd->rotation);

	// Reset the processed image:
	processed_reset(state);

	// Account for rotation:
	if (fd->rotation != 0)
		state->processed = gdk_pixbuf_rotate_simple(fd->pixbuf, fd->rotation);

	// Recalculate layout:
	geometry_zoom(&state->geometry, fd->zoom_factor);

	// For non-unity zoom factors, resize pixbuf:
	if (fd->zoom_factor != 1.0f) {

		// Interpolation mode:
		GdkInterpType mode = (fd->zoom_factor < 1.0f)
			? GDK_INTERP_BILINEAR
			: GDK_INTERP_NEAREST;

		// If image is zoomed, create a new pixbuf:
		GdkPixbuf *zoomed = gdk_pixbuf_scale_simple(
			(state->processed) ? state->processed : fd->pixbuf,
			state->geometry.pixbuf_wd,
			state->geometry.pixbuf_ht,
			mode);

		// Reset the processed image:
		processed_reset(state);

		// Put zoomed pixbuf in its place:
		state->processed = zoomed;
	}

	// Bound the window dimensions to the screen dimensions:
	if (state->geometry.window_wd > state->screen_wd)
		geometry_window_resized(&state->geometry,
			state->screen_wd,
			state->geometry.window_ht);

	if (state->geometry.window_ht > state->screen_ht)
		geometry_window_resized(&state->geometry,
			state->geometry.window_wd,
			state->screen_ht);

	// Resize main window to the new geometry:
	gtk_window_resize(GTK_WINDOW(state->window),
		state->geometry.window_wd,
		state->geometry.window_ht);

	// Get feedback on the window's actual size.
	// If the window is maximized, resizing it has no effect,
	// and we would be assuming the wrong window size:
	GtkAllocation allocation;
	gtk_widget_get_allocation(state->window, &allocation);
	geometry_window_resized(&state->geometry,
		allocation.width,
		allocation.height);

	// Request a redraw:
	gtk_widget_queue_draw(state->darea);
}

// Load image into window
static void
gui_load (struct state *state)
{
	struct filedata *fd = state->file->data;

	geometry_reset(&state->geometry, fd->pixbuf);
	gtk_window_set_title(GTK_WINDOW(state->window), fd->path);

	// Zoom in/out by stored factor:
	processed_create(state);
}

// This link will be deleted shortly; move on:
void
gui_notify_deleted (GList *file, struct state *state)
{
	GList *next;

	// If we're not displaying this file, never mind:
	if (file != state->file)
		return;

	// Try to find next file in list; if this fails, go in reverse:
	if (!(next = filelist_scan_forward(state->list, file->next)))
		if (!(next = filelist_scan_backward(state->list, file->prev))) {
			gtk_main_quit();
			return;
		}

	// Move to the new file:
	state->file = next;
	gui_load(state);
}

// This link has been changed; reload:
void
gui_notify_changed (GList *file, struct state *state)
{
	GList *next;
	struct filedata *fd = state->file->data;

	// If we're not displaying this file, never mind:
	if (file != state->file)
		return;

	// Destroy file's pixbuf to force a reload:
	g_object_unref(fd->pixbuf);
	fd->pixbuf = NULL;

	// Try to find next file in list; if this fails, go in reverse:
	if (!(next = filelist_scan_forward(state->list, file)))
		if (!(next = filelist_scan_backward(state->list, file->prev))) {
			gtk_main_quit();
			return;
		}

	// Reload the file:
	state->file = next;
	gui_load(state);
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

// Zoom pixbuf in
static void
zoom_in (struct state *state)
{
	struct filedata *fd = state->file->data;

	// Set zoom factor:
	if (fd->zoom_factor == zoom_table[zoom_table_size - 1])
		return;

	for (size_t i = 0; i < zoom_table_size - 1; i++)
		if (fd->zoom_factor == zoom_table[i]) {
			fd->zoom_factor = zoom_table[i + 1];
			break;
		}

	// Create processed image;
	processed_create(state);
}

// Zoom pixbuf out
static void
zoom_out (struct state *state)
{
	struct filedata *fd = state->file->data;

	// Set zoom factor:
	if (fd->zoom_factor == zoom_table[0])
		return;

	for (size_t i = 1; i < zoom_table_size; i++)
		if (fd->zoom_factor == zoom_table[i]) {
			fd->zoom_factor = zoom_table[i - 1];
			break;
		}

	// Create processed image:
	processed_create(state);
}

// Zoom to natural size
static void
zoom_natural (struct state *state)
{
	struct filedata *fd = state->file->data;

	// Set natural zoom factor:
	fd->zoom_factor = 1.0f;

	// Create processed image:
	processed_create(state);
}

// Rotate counterclockwise
static void
rotate_ccw (struct state *state)
{
	struct filedata *fd = state->file->data;

	fd->rotation += 90;

	if (fd->rotation > 270)
		fd->rotation = 0;

	// Create processed image:
	processed_create(state);
}

// Rotate clockwise
static void
rotate_cw (struct state *state)
{
	struct filedata *fd = state->file->data;

	fd->rotation -= 90;

	if (fd->rotation < 0)
		fd->rotation = 270;

	// Create processed image:
	processed_create(state);
}

// Delete current image
static void
delete (struct state *state)
{
	struct filedata *fd = state->file->data;

	GtkWidget *dialog = gtk_message_dialog_new(
		GTK_WINDOW(state->window),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_QUESTION,
		GTK_BUTTONS_YES_NO,
		"Delete %s?",
		fd->basename);

	// Run dialog, remove the file if user confirms:
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES) {

		// Attempt to remove file:
		if (g_unlink(fd->path) == 0) {
			GList *file = state->file;

			// Notify GUI that file will go away:
			gui_notify_deleted(file, state);

			// Delete file from list:
			filelist_unlink(state->list, file);
		}
	}

	gtk_widget_destroy(dialog);
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

	case GDK_KEY_plus:
		zoom_in(state);
		break;

	case GDK_KEY_minus:
		zoom_out(state);
		break;

	case GDK_KEY_7:
		rotate_ccw(state);
		break;

	case GDK_KEY_9:
		rotate_cw(state);
		break;

	case GDK_KEY_Delete:
		delete(state);
		break;

	case GDK_KEY_o:
	case GDK_KEY_O:
		zoom_natural(state);
		break;

	case GDK_KEY_q:
	case GDK_KEY_Q:
		gtk_main_quit();
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
		(event->state & GDK_CONTROL_MASK)
			? zoom_in(state)
			: move_to_prev(state);
		break;

	case GDK_SCROLL_DOWN:
		(event->state & GDK_CONTROL_MASK)
			? zoom_out(state)
			: move_to_next(state);
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

	// Use original pixbuf or processed one if available:
	gdk_cairo_set_source_pixbuf(cr,
		(state->processed) ? state->processed : fd->pixbuf,
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
			drag_cursor_set(state);
		}

	return FALSE;
}

// Mouse button was released
static gboolean
on_button_release (GtkWidget *widget, GdkEventButton *event, struct state *state)
{
	if (event->button == 1) {
		state->panning = FALSE;
		drag_cursor_remove(state);
	}

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
gui_run (GList **list, GList *file, const gchar *dir)
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

	// Load application icons:
	icons_load();

	// Show window:
	gtk_widget_show_all(state.window);

	// Get screen geometry:
	GdkScreen *screen = gtk_window_get_screen(GTK_WINDOW(state.window));
	state.screen_wd = gdk_screen_get_width(screen);
	state.screen_ht = gdk_screen_get_height(screen);

	// Load initial image:
	gui_load(&state);

	// Create file monitor:
	monitor_create(state.list, dir, &state);

	// Enter main loop:
	gtk_main();

	// Destroy file monitor:
	monitor_destroy();

	// Free processed pixbuf:
	processed_reset(&state);
}

// Initialize GTK
void
gui_init (int argc, char *argv[])
{
	gtk_init(&argc, &argv);
}
