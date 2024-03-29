# akview

[![Build](https://github.com/aklomp/akview/actions/workflows/build.yml/badge.svg)](https://github.com/aklomp/akview/actions/workflows/build.yml)

A simple and fast image viewer inspired by Kuickshow.

## Features

- Easy to use, copies Kuickshow's main key and mouse bindings;
- Shows just the image, no extra clutter;
- Silently skips files it can't decode;
- Remembers zoom and rotation settings (within current session);
- Monitors directory for deleted, changed and new files;
- Can zoom and rotate images, but never touches the originals;
- Can delete images;
- Can be run on a directory or an individual file;
- Simple, clean C code;
- Written for plain, modern Gtk+ 3.

## Arguments

If you run akview without arguments, it will display the first image in the current directory.
If the first argument is a directory, akview will start on the first image in that directory.
If the first argument is a full path or a filename, akview will start on that image.
It's always possible to move between images in the same directory.
Deletions, additions and modifications are monitored and processed in real time.

## Key bindings

- Home: move to first image in list.
- End: move to last image in list.
- Page Up: move to previous image.
- Page Down: move to next image.
- Delete: delete current image (after asking confirmation).
- +: zoom in.
- -: zoom out.
- 7: rotate counterclockwise.
- 9: rotate clockwise.
- o: zoom to natural size (1:1).
- q: quit program.

The mouse scroll wheel also moves between images.
Ctrl + mouse scroll zooms.

## Installation

Run `make` to generate the `akview` binary.

Run `sudo make install` to install the binary and the .desktop file to default locations.

Run `sudo make install DESTDIR=/tmp/akview` to install to a custom directory for packaging.

Run `sudo make install PREFIX=/usr` to use a custom install prefix. `PREFIX` and `DESTDIR` can be combined.

## License

This program is distributed under the terms of the
[GNU General Public License version 3](https://www.gnu.org/licenses/gpl-3.0.html).
See COPYING for the full license text.
