CFLAGS += -std=c99 -Werror -Wall -pedantic

CFLAGS_GLIB  = `pkg-config --cflags glib-2.0`
LDFLAGS_GLIB = `pkg-config --libs glib-2.0`

CFLAGS  += $(CFLAGS_GLIB)
LDFLAGS += $(LDFLAGS_GLIB)

OBJS = \
  src/main.o

.PHONY: clean

akview: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

clean:
	rm -f $(OBJS) akview
