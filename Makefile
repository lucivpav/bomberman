CC=clang++
LDFLAGS=-lncurses -lpthread
CFLAGS=-std=c++11 -Wall -pedantic -Wno-long-long -O0 -ggdb

all: bomberman

tests: test

OBJECTS=map.o block.o game.o pos.o countdown.o player.o enemy.o ai_player.o online_player.o ui.o mainmenu.o singleplayermenu.o multiplayermenu.o hostmenu.o connectmenu.o bomb.o ghost.o trap.o client.o server.o clientgame.o

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

enemy.o: src/enemy.cpp
	$(CC) -c $(CFLAGS) src/enemy.cpp

ai_player.o: src/ai_player.cpp
	$(CC) -c $(CFLAGS) src/ai_player.cpp

online_player.o: src/online_player.cpp
	$(CC) -c $(CFLAGS) src/online_player.cpp

ui.o: src/ui.cpp
	$(CC) -c $(CFLAGS) src/ui.cpp

mainmenu.o: src/mainmenu.cpp
	$(CC) -c $(CFLAGS) src/mainmenu.cpp

singleplayermenu.o: src/singleplayermenu.cpp
	$(CC) -c $(CFLAGS) src/singleplayermenu.cpp

multiplayermenu.o: src/multiplayermenu.cpp
	$(CC) -c $(CFLAGS) src/multiplayermenu.cpp

hostmenu.o: src/hostmenu.cpp
	$(CC) -c $(CFLAGS) src/hostmenu.cpp

connectmenu.o: src/connectmenu.cpp
	$(CC) -c $(CFLAGS) src/connectmenu.cpp

bomb.o: src/bomb.cpp
	$(CC) -c $(CFLAGS) src/bomb.cpp

ghost.o: src/ghost.cpp
	$(CC) -c $(CFLAGS) src/ghost.cpp

trap.o: src/trap.cpp
	$(CC) -c $(CFLAGS) src/trap.cpp

client.o: src/client.cpp
	$(CC) -c $(CFLAGS) src/client.cpp

server.o: src/server.cpp
	$(CC) -c $(CFLAGS) src/server.cpp

clientgame.o: src/clientgame.cpp
	$(CC) -c $(CFLAGS) src/clientgame.cpp

clean:
	-rm *.o bomberman test 2>/dev/null || true
