CC=clang++
LDFLAGS=-lncurses
CFLAGS=-std=c++11 -Wall -pedantic -Wno-long-long -O0 -ggdb

all: bomberman

tests: test

bomberman: main.o map.o block.o
	$(CC) $(CFLAGS) main.o map.o block.o -o bomberman $(LDFLAGS)

test: test.o map.o block.o
	$(CC) $(CFLAGS) test.o map.o block.o -o test $(LDFLAGS)

test.o: tests/test.cpp
	$(CC) -c $(CFLAGS) tests/test.cpp

main.o: main.cpp
	$(CC) -c $(CFLAGS) main.cpp

map.o: map.cpp
	$(CC) -c $(CFLAGS) map.cpp

block.o: block.cpp
	$(CC) -c $(CFLAGS) block.cpp

clean:
	rm *.o bomberman test
