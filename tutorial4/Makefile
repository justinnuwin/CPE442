CC=g++
PROGRAM_NAME=sobel
CFLAGS=-Wall -Werror -g -fopenmp
LDFLAGS=-lopencv_core -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lm -lpthread
C_HEADERS=grayscale.hpp sobel.hpp
C_SOURCES=main.cpp grayscale.cpp sobel.cpp

all: $(C_HEADERS) $(C_SOURCES)
	$(CC) -o $(PROGRAM_NAME) $(CFLAGS) $(C_SOURCES) $(LDFLAGS)

sobel.o: sobel.cpp sobel.hpp
	$(CC) -o $@ $(CFLAGS) -c $< $(LDFLAGS)

grayscale.o: grayscale.cpp grayscale.hpp
	$(CC) -o $@ $(CFLAGS) -c $< $(LDFLAGS)

test: simd simd_openmp

simd: simd_test.cpp
	$(CC) -o $@ -g $<

simd_openmp: simd_test.cpp
	$(CC) -o $@ -g -fopenmp $<


.PHONY: clean

clean:
	rm -f *.o
	rm -f $(PROGRAM_NAME)
	rm -f simd simd_openmp
