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
