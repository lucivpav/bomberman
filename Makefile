CC=clang++
LDFLAGS=-lncurses
CFLAGS=-std=c++11 -Wall -pedantic -Wno-long-long -O0 -ggdb

all: bomberman

tests: test

OBJECTS=map.o block.o game.o pos.o countdown.o player.o ai_player.o menu.o mainmenu.o bomb.o ghost.o trap.o

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

countdown.o: src/countdown.cpp
	$(CC) -c $(CFLAGS) src/countdown.cpp

player.o: src/player.cpp
	$(CC) -c $(CFLAGS) src/player.cpp

ai_player.o: src/ai_player.cpp
	$(CC) -c $(CFLAGS) src/ai_player.cpp

menu.o: src/menu.cpp
	$(CC) -c $(CFLAGS) src/menu.cpp

mainmenu.o: src/mainmenu.cpp
	$(CC) -c $(CFLAGS) src/mainmenu.cpp

bomb.o: src/bomb.cpp
	$(CC) -c $(CFLAGS) src/bomb.cpp

ghost.o: src/ghost.cpp
	$(CC) -c $(CFLAGS) src/ghost.cpp

trap.o: src/trap.cpp
	$(CC) -c $(CFLAGS) src/trap.cpp

clean:
	-rm *.o bomberman test 2>/dev/null || true
