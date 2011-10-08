CC=gcc
CFLAGS= -g -Wall -Wextra -pedantic -std=c99
LDFLAGS= -lGL -lGLU -lglut
LBITS := $(shell getconf LONG_BIT)
ifeq ($(LBITS),64)
   LINKFLAG= /lib64/libm.so.6
else
   LINKFLAG=
endif
SOURCES=  main.c game.c input.c graphic.c array.c ai.c fifo.c menu.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=bin/game

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(LINKFLAG)  $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) $< -c -o $@
	
clean: 
	rm -f $(OBJECTS); 
