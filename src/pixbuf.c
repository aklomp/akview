#include <gdk/gdk.h>

#include "filedata.h"

// Create a pixbuf for the given path
gboolean
pixbuf_create (struct filedata *fd)
{
	// Delete existing pixbuf, if any:
	if (fd->pixbuf)
		g_object_unref(fd->pixbuf);

	// Attempt to create pixbuf from path:
	if (!(fd->pixbuf = gdk_pixbuf_new_from_file(fd->path, NULL)))
		return FALSE;

	// The pixbuf has been loaded:
	return TRUE;
}
