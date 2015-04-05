CC=clang++
LDFLAGS=-lncurses
CFLAGS=-std=c++11 -Wall -pedantic -Wno-long-long -O0 -ggdb

all: bomberman

tests: test

OBJECTS=map.o block.o game.o pos.o player.o

bomberman: main.o $(OBJECTS)
	$(CC) $(CFLAGS) main.o $(OBJECTS) -o bomberman $(LDFLAGS)

test: test.o $(OBJECTS)
	$(CC) $(CFLAGS) test.o $(OBJECTS) -o test $(LDFLAGS)

test.o: tests/test.cpp
	$(CC) -c $(CFLAGS) tests/test.cpp

main.o: main.cpp
	$(CC) -c $(CFLAGS) main.cpp

map.o: map.cpp
	$(CC) -c $(CFLAGS) map.cpp

block.o: block.cpp
	$(CC) -c $(CFLAGS) block.cpp

game.o: game.cpp
	$(CC) -c $(CFLAGS) game.cpp

pos.o: pos.cpp
	$(CC) -c $(CFLAGS) pos.cpp

player.o: player.cpp
	$(CC) -c $(CFLAGS) player.cpp

clean:
	rm *.o bomberman test
