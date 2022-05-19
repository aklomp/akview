BIN  := akview

CFLAGS += -std=c99 -Werror -Wall -pedantic
CFLAGS += $(shell pkg-config --cflags gtk+-3.0)
LIBS   += $(shell pkg-config --libs   gtk+-3.0)

PREFIX ?= /usr/local

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)
OBJS += \
  res/icon-16.o \
  res/icon-32.o \
  res/icon-48.o \
  res/icon-64.o \
  res/icon-128.o

.PHONY: analyze clean install

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

res/icon-%.o: res/icon-%.png
	$(LD) --relocatable --format=binary -o $@ $^

res/icon-%.png: res/icon.svg
	rsvg-convert --format png --width $* --output $@ $^

install: $(BIN)
	install -D -m 0755 $(BIN) $(DESTDIR)$(PREFIX)/bin/$(BIN)
	install -D -m 0644 res/akview.desktop $(DESTDIR)$(PREFIX)/share/applications/akview.desktop

analyze: clean
	scan-build --status-bugs $(MAKE)

clean:
	$(RM) $(OBJS) $(BIN)
