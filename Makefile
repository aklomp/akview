CFLAGS += -std=c99 -Werror -Wall -pedantic

OBJS = \
  src/main.o

.PHONY: clean

akview: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

clean:
	rm -f $(OBJS) akview
