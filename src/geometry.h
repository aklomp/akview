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

struct size {
	int width;
	int height;
};

struct position {
	int x;
	int y;
};

struct geometry
{
	struct {
		struct size native;
		struct size active;
	} pixbuf;
	struct size window;
	struct position offset;
	struct position pan_start;
	gfloat zoom_factor;
	int rotation;
};

void geometry_reset (struct geometry *g, GdkPixbuf *pixbuf);
void geometry_window_resized (struct geometry *g, const struct size *size);
void geometry_pan_start (struct geometry *g, const struct position *pos);
void geometry_pan_update (struct geometry *g, const struct position *pos);
void geometry_zoom (struct geometry *g, gfloat zoom_factor);
void geometry_rotate (struct geometry *g, int rotation);
