CC=gcc
PROGRAM_NAME=hello_world
CFLAGS=-Wall -Werror -O1 -g
LDFLAGS=-lm
C_HEADERS=second_source.h
C_SOURCES=hello_world.c second_source.c

all: $(C_HEADERS) $(C_SOURCES)
	$(CC) -o $(PROGRAM_NAME) $(CFLAGS) $(C_SOURCES) $(LDFLAGS)

.PHONY: clean

clean:
	rm -f *.o
	rm -f $(PROGRAM_NAME)
