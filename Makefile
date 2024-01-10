CC = gcc
CFLAGS = -Wall -Wextra -Werror -ggdb
LIBS = -lSDL2 -lcurses

c8: main.c chip.c render.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f c8

all: c8

install: c8
	cp c8 /usr/local/bin