# Bomberman #
[![Build Status](https://travis-ci.org/lucivpav/bomberman.svg?branch=master)](https://travis-ci.org/lucivpav/bomberman)

A command line Bomberman game written in C++.

![](http://i.imgur.com/QE7mLYm.gif)

### Features ###
* Singleplayer (including ghosts, traps, ...)
* Multiplayer (over a network)
* Bonuses (increased flame length, multiple bombs, remote bombs, faster movement)
* AI
* GUI mode (--gui)
* Editor

### Controls ###
* WASD/arrows move
* B place bomb
* space detonate bombs
* Q exit

### Dependencies ###
* pthread
* ncurses
* unix sockets
* SDL2, SDL2_image, SDL2_ttf
* doxygen (optional)

### Compiling on Ubuntu ###
```Bash
make compile # to compile the game 
# OR
make # to compile the game, docs and tests
```

### Running ###
```Bash
./bomberman
# OR
./bomberman --gui
```
User is responsible for ensuring the terminal size is at least 
47x24 characters during program execution. The program is not
guaranteed to function properly if this condition is not met.
