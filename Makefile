CFLAGS += -std=c99 -Werror -Wall -pedantic

CFLAGS_GDK = `pkg-config --cflags gdk-2.0`
LDFLAGS_GDK = `pkg-config --libs gdk-2.0`

CFLAGS  += $(CFLAGS_GDK)
LDFLAGS += $(LDFLAGS_GDK)

OBJS = \
  src/filedata.o \
  src/filelist.o \
  src/main.o

.PHONY: clean

akview: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

clean:
	rm -f $(OBJS) akview
