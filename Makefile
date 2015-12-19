CFLAGS += -std=c99 -Werror -Wall -pedantic

CFLAGS_GTK  := $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS_GTK := $(shell pkg-config --libs   gtk+-3.0)

CFLAGS  += $(CFLAGS_GTK)
LDFLAGS += $(LDFLAGS_GTK)

PREFIX ?= /usr/local

OBJS = \
  res/icon-16.o \
  res/icon-32.o \
  res/icon-48.o \
  res/icon-64.o \
  res/icon-128.o \
  $(patsubst %.c,%.o,$(wildcard src/*.c))

.PHONY: analyze clean install

akview: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -c $^

res/icon-%.o: res/icon-%.png
	$(LD) --relocatable --format=binary -o $@ $^

res/icon-%.png: res/icon.svg
	rsvg-convert --format png --width $* --output $@ $^

install: akview
	install -D -m 0755 akview $(DESTDIR)$(PREFIX)/bin/akview
	install -D -m 0644 res/akview.desktop $(DESTDIR)$(PREFIX)/share/applications/akview.desktop

analyze: clean
	scan-build --status-bugs make

clean:
	rm -f $(OBJS) akview
