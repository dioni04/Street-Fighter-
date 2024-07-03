CC = gcc
COMP_FLAGS = -Wall $(shell pkg-config --cflags allegro-5 allegro_font-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 allegro_image-5 allegro_ttf-5)
LINK_FLAGS = $(shell pkg-config --libs allegro-5 allegro_font-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 allegro_image-5 allegro_ttf-5)
PROG_NAME = street

all: $(PROG_NAME)

$(PROG_NAME): main.o characters.o draw.o destroy.o control.o movement.o
	$(CC) main.o characters.o draw.o destroy.o control.o movement.o $(LINK_FLAGS) -o $(PROG_NAME)
main.o: main.c
	$(CC) main.c -c $(COMP_FLAGS)
characters.o: characters.c
	$(CC) characters.c -c $(COMP_FLAGS)
draw.o: draw.c
	$(CC) draw.c -c $(COMP_FLAGS)
destroy.o: destroy.c
	$(CC) destroy.c -c $(COMP_FLAGS)
movement.o: movement.c
	$(CC) movement.c -c $(COMP_FLAGS)
control.o: control.c
	$(CC) control.c -c $(COMP_FLAGS)
purge: clean
	rm -f $(PROG_NAME)
clean:
	rm -f *.o
