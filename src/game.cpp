#include "game.h"

#include <ncurses.h>

Game::Game()
{
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    cbreak();
    curs_set(FALSE);
    timeout(33);

    map.load("levels/1");
    loop();
}

Game::~Game()
{
    endwin();
}

void Game::loop()
{
    while (1)
    {
        int c = getch();
        if ( c == 'q' )
            break;
        map.keyEvent(c);
        map.draw();
        refresh();
    }
}
