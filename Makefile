CFLAGS += -std=c99 -Werror -Wall -pedantic

CFLAGS_GTK  = `pkg-config --cflags gtk+-3.0`
LDFLAGS_GTK = `pkg-config --libs gtk+-3.0`

CFLAGS  += $(CFLAGS_GTK)
LDFLAGS += $(LDFLAGS_GTK)

OBJS = \
  src/filedata.o \
  src/filelist.o \
  src/gui.o \
  src/main.o \
  src/pixbuf.o

.PHONY: clean

akview: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

clean:
	rm -f $(OBJS) akview
