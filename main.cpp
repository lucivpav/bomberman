#include <ncurses.h>
#include <unistd.h>

#include "map.h"

int main(int argc, char ** argv)
{
    Map map("levels/1");

    initscr();
    noecho();
    curs_set(FALSE);
    map.draw();
    refresh();
    getch();
    endwin();

    return 0;
}
