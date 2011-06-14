CC=gcc
CFLAGS= -g -Wall
LDFLAGS= -lGL -lGLU -lglut 
SOURCES=  main.c game.c input.c graphic.c array.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=bin/game

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) /lib64/libm.so.6 $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) $< -c -o $@
	
clean: 
	rm -f $(OBJECTS); 