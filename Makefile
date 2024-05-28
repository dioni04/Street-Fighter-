CC = gcc
COMP_FLAGS = -Wall $(shell pkg-config --cflags allegro-5 allegro_font-5)
LINK_FLAGS = $(shell pkg-config --libs allegro-5 allegro_font-5)
PROG_NAME = street

all: main street.h
	$(CC) main.o $(LINK_FLAGS) -o $(PROG_NAME)
main: main.c
	$(CC) main.c -c $(COMP_FLAGS)
purge: clean
	rm -f $(PROG_NAME)
clean:
	rm -f *.o
