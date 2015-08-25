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

struct filedata {
	gchar *path;
	gchar *basename;
	gchar *collate_key;
	GdkPixbuf *pixbuf;
	gfloat zoom_factor;
	gint rotation;
};

struct filedata *filedata_create (gchar *path);
void filedata_destroy (struct filedata *fd);
gint filedata_compare (gconstpointer a, gconstpointer b);
