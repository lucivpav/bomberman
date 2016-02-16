### About ###
A command line Bomberman game written in C++ using ncurses and pthreads.

### Features ###
* Singleplayer (including ghosts, traps, ...)
* Multiplayer (over a network)
* Bonuses (increased flame length, multiple bombs, remote bombs, faster movement)
* AI

### Controls ###
* WASD move
* B place bomb
* space detonate bombs
* Q exit

### Dependencies ###
* pthread
* ncurses
* doxygen (optional)

### Compiling on Ubuntu ###
```Bash
apt-get install libncurses-dev

make compile # to compile the game 
# OR
apt-get install doxygen
make # to compile the game, docs and tests
```

### Running ###
```Bash
./bomberman
```
User is responsible for ensuring the terminal size is at least 
80x24 characters during program execution. The program is not
guaranteed to function properly if this condition is not met.
