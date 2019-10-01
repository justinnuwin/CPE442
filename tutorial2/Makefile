CC=g++
PROGRAM_NAME=a2
CFLAGS=-Wall -Werror -g
LDFLAGS=-lopencv_core -lopencv_highgui -lopencv_videoio -lm	-lopencv_imgcodecs
C_HEADERS=grayscale.hpp sobel.hpp
C_SOURCES=main.cpp grayscale.cpp sobel.cpp

all: $(C_HEADERS) $(C_SOURCES)
	$(CC) -o $(PROGRAM_NAME) $(CFLAGS) $(C_SOURCES) $(LDFLAGS)

.PHONY: clean

clean:
	rm -f *.o
	rm -f $(PROGRAM_NAME)
