#include "mainmenu.h"

#include <ncurses.h>

#include "game.h"
#include "levelmenu.h"

MainMenu::MainMenu(const char * argv0)
    :Menu()
{
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    cbreak();
    curs_set(FALSE);
    timeout(33);

    SingleplayerItem  * singleplayer = new SingleplayerItem("SINGLEPLAYER", argv0);
    MultiplayerItem  * multiplayer = new MultiplayerItem("MULTIPLAYER");
    ExitItem * exit = new ExitItem("EXIT");

    addItem(singleplayer);
    addItem(multiplayer);
    addItem(exit);

    loop();
}


MainMenu::~MainMenu()
{
    endwin();
}


SingleplayerItem::SingleplayerItem(const char *name, const char * argv0)
    :MenuItem(name), mArgv0(argv0)
{
}

bool SingleplayerItem::action()
{
    LevelMenu levelMenu(mArgv0);
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
