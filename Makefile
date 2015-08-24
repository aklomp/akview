CFLAGS += -std=c99 -Werror -Wall -pedantic

CFLAGS_GTK  = `pkg-config --cflags gtk+-3.0`
LDFLAGS_GTK = `pkg-config --libs gtk+-3.0`

CFLAGS  += $(CFLAGS_GTK)
LDFLAGS += $(LDFLAGS_GTK)

PREFIX ?= /usr/local

OBJS = \
  src/filedata.o \
  src/filelist.o \
  src/geometry.o \
  src/gui.o \
  src/main.o \
  src/monitor.o \
  src/pixbuf.o

.PHONY: analyze clean install

akview: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

install: akview
	install -D -m 0755 akview $(DESTDIR)$(PREFIX)/bin/akview
	install -D -m 0644 res/akview.desktop $(DESTDIR)$(PREFIX)/share/applications/akview.desktop

analyze: clean
	scan-build --status-bugs make

clean:
	rm -f $(OBJS) akview
