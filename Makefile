LDFLAGS=-lncurses -lpthread
CFLAGS=-std=c++11 -Wall -pedantic -Wno-long-long -O0 -ggdb
OBJECTS=bomberman.o map.o block.o game.o pos.o countdown.o player.o enemy.o ai_player.o online_player.o ui.o mainmenu.o singleplayermenu.o multiplayermenu.o hostmenu.o connectmenu.o bonus.o bomb.o ghost.o trap.o client.o server.o clientgame.o

all: compile tests doc

tests: test

doc:
	doxygen

run:
	./bomberman

compile: main.o $(OBJECTS)
	$(CXX) $(CFLAGS) main.o $(OBJECTS) -o bomberman $(LDFLAGS)

test: test.o $(OBJECTS)
	$(CXX) $(CFLAGS) test.o $(OBJECTS) -o test $(LDFLAGS)

test.o: tests/test.cpp
	$(CXX) -c $(CFLAGS) tests/test.cpp

main.o: src/main.cpp src/bomberman.h
	$(CXX) -c $(CFLAGS) src/main.cpp

bomberman.o: src/bomberman.cpp src/bomberman.h src/mainmenu.h
	$(CXX) -c $(CFLAGS) src/bomberman.cpp

map.o: src/map.cpp src/map.h
	$(CXX) -c $(CFLAGS) src/map.cpp

block.o: src/block.cpp src/block.h
	$(CXX) -c $(CFLAGS) src/block.cpp

game.o: src/game.cpp src/game.h src/ui.h src/ai_player.h src/online_player.h
	$(CXX) -c $(CFLAGS) src/game.cpp

pos.o: src/pos.cpp src/pos.h
	$(CXX) -c $(CFLAGS) src/pos.cpp

countdown.o: src/countdown.cpp src/countdown.h src/game.h
	$(CXX) -c $(CFLAGS) src/countdown.cpp

player.o: src/player.cpp src/player.h src/game.h
	$(CXX) -c $(CFLAGS) src/player.cpp

enemy.o: src/enemy.cpp src/enemy.h src/block.h
	$(CXX) -c $(CFLAGS) src/enemy.cpp

ai_player.o: src/ai_player.cpp src/ai_player.h src/game.h src/block.h
	$(CXX) -c $(CFLAGS) src/ai_player.cpp

online_player.o: src/online_player.cpp src/online_player.h src/game.h src/server.h src/client.h
	$(CXX) -c $(CFLAGS) src/online_player.cpp

ui.o: src/ui.cpp src/ui.h
	$(CXX) -c $(CFLAGS) src/ui.cpp

mainmenu.o: src/mainmenu.cpp src/mainmenu.h src/singleplayermenu.h src/multiplayermenu.h
	$(CXX) -c $(CFLAGS) src/mainmenu.cpp

singleplayermenu.o: src/singleplayermenu.cpp  src/singleplayermenu.h src/game.h
	$(CXX) -c $(CFLAGS) src/singleplayermenu.cpp

multiplayermenu.o: src/multiplayermenu.cpp src/multiplayermenu.h src/hostmenu.h src/connectmenu.h
	$(CXX) -c $(CFLAGS) src/multiplayermenu.cpp

hostmenu.o: src/hostmenu.cpp src/hostmenu.h src/game.h
	$(CXX) -c $(CFLAGS) src/hostmenu.cpp

connectmenu.o: src/connectmenu.cpp src/connectmenu.h src/clientgame.h
	$(CXX) -c $(CFLAGS) src/connectmenu.cpp

bonus.o: src/bonus.cpp src/bonus.h
	$(CXX) -c $(CFLAGS) src/bonus.cpp

bomb.o: src/bomb.cpp src/bomb.h src/player.h
	$(CXX) -c $(CFLAGS) src/bomb.cpp

ghost.o: src/ghost.cpp src/ghost.h src/game.h
	$(CXX) -c $(CFLAGS) src/ghost.cpp

trap.o: src/trap.cpp src/trap.h src/map.h
	$(CXX) -c $(CFLAGS) src/trap.cpp

client.o: src/client.cpp src/client.h src/game.h
	$(CXX) -c $(CFLAGS) src/client.cpp

server.o: src/server.cpp src/server.h src/client.h
	$(CXX) -c $(CFLAGS) src/server.cpp

clientgame.o: src/clientgame.cpp src/clientgame.h src/ui.h src/countdown.h
	$(CXX) -c $(CFLAGS) src/clientgame.cpp

clean:
	rm -f *.o bomberman test
	rm -rf doc
