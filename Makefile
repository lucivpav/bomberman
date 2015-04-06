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

main.o: src/main.cpp
	$(CC) -c $(CFLAGS) src/main.cpp

map.o: src/map.cpp
	$(CC) -c $(CFLAGS) src/map.cpp

block.o: src/block.cpp
	$(CC) -c $(CFLAGS) src/block.cpp

game.o: src/game.cpp
	$(CC) -c $(CFLAGS) src/game.cpp

pos.o: src/pos.cpp
	$(CC) -c $(CFLAGS) src/pos.cpp

player.o: src/player.cpp
	$(CC) -c $(CFLAGS) src/player.cpp

clean:
	rm *.o bomberman test
