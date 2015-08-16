#include <glib.h>
#include <gio/gio.h>

#include "filelist.h"
#include "gui.h"

struct state {
	GList **list;
	void *data;
};

static GFile *dir = NULL;
static GFileMonitor *monitor = NULL;

static void
on_changed (GFileMonitor *monitor, GFile *file, GFile *other, GFileMonitorEvent type, struct state *state)
{
	switch (type) {
	case G_FILE_MONITOR_EVENT_CHANGED: {
		GList *link;
		gchar *path = g_file_get_path(file);

		if ((link = filelist_find(*state->list, path)) != NULL)
			gui_notify_changed(link, state->data);

		g_free(path);
		break;
	}

	case G_FILE_MONITOR_EVENT_DELETED: {
		GList *link;
		gchar *path = g_file_get_path(file);

		if ((link = filelist_find(*state->list, path)) != NULL) {
			gui_notify_deleted(link, state->data);
			filelist_unlink(state->list, link);
		}

		g_free(path);
		break;
	}

	case G_FILE_MONITOR_EVENT_CREATED:
		filelist_link(state->list, g_file_get_path(file));
		break;

	default:
		break;
	}
}

void
monitor_destroy (void)
{
	if (dir)
		g_object_unref(dir);

	if (monitor)
		g_object_unref(monitor);
}

void
monitor_create (GList **list, const char *path, void *data)
{
	static struct state state;

	state.list = list;
	state.data = data;

	dir = g_file_new_for_path(path);

	if (!(monitor = g_file_monitor_directory(dir, G_FILE_MONITOR_NONE, NULL, NULL)))
		return;

	g_signal_connect(monitor, "changed", G_CALLBACK(on_changed), &state);
}
