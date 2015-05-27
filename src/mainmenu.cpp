#include "mainmenu.h"

#include <ncurses.h>

#include "game.h"

MainMenu::MainMenu()
{
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    cbreak();
    curs_set(FALSE);
    timeout(33);

    SingleplayerItem  * singleplayer = new SingleplayerItem("SINGLEPLAYER");
    MultiplayerItem  * multiplayer = new MultiplayerItem("MULTIPLAYER");
    ExitItem * exit = new ExitItem("EXIT");

    menu.addItem(singleplayer);
    menu.addItem(multiplayer);
    menu.addItem(exit);

    menu.loop();
}


MainMenu::~MainMenu()
{
    endwin();
}


SingleplayerItem::SingleplayerItem(const char *name)
    :MenuItem(name)
{
}

bool SingleplayerItem::action()
{
    Game game(true);
    return false;
}


MultiplayerItem::MultiplayerItem(const char *name)
    :MenuItem(name)
{
}

bool MultiplayerItem::action()
{
    return false;
}


ExitItem::ExitItem(const char *name)
    :MenuItem(name)
{
}

bool ExitItem::action()
{
    return true;
}
