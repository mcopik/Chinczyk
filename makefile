CC=gcc
CFLAGS= -g -Wall
LDFLAGS= -lGL -lGLU -lglut /lib64/libm.so.6
SOURCES=  main.c game.c input.c graphic.c array.c ai.c fifo.c menu.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=bin/game

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS)  $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) $< -c -o $@
	
clean: 
	rm -f $(OBJECTS); 
