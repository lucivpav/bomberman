#include "levelmenu.h"

#include "game.h"

#include <fstream>
#include <ncurses.h>

std::string LevelItem::mLevelsPath;
const LevelMenu * LevelItem::mMenu;

LevelMenu::LevelMenu(const std::string & argv0)
    :Menu(), mArgv0(argv0), mTrapsEnabled(true)
{
    /*
         * gcc - FAIL
         * ./gcc
         * ope/asf/gcc
         * ../.././../asf/ff/gcc
         * ../gcc
         */
    std::string path = argv0.substr(0, argv0.rfind("bomberman")) + "levels/";
    for ( int i = 1 ; i <= 10 ; i++ )
    {
        std::ifstream in(path + std::to_string(i));
        if ( !in )
            break;
        addItem(new LevelItem(std::string("LEVEL " + std::to_string(i)).c_str(),
                              path,
                              *this));
    }
    loop();
}

void LevelMenu::keyEvent(int key)
{
    Menu::keyEvent(key);
    if ( key == 't' )
        mTrapsEnabled = !mTrapsEnabled;
}

void LevelMenu::drawEvent()
{
    Menu::drawEvent();
    int width, height;
    getmaxyx(stdscr, height, width);
    std::string msg = std::string("TRAPS ") + (mTrapsEnabled ? "ENABLED" : "DISABLED" )
            + " (press T to toggle)";
    mvprintw(height-1, 0, msg.c_str());
}

bool LevelMenu::trapsEnabled() const
{
    return mTrapsEnabled;
}

LevelItem::LevelItem(const char *name,
                     const std::string & levelsPath,
                     const LevelMenu & menu)
    :MenuItem(name)
{
    if ( mLevelsPath.empty() )
        mLevelsPath = levelsPath;
    mMenu = &menu;
}

bool LevelItem::action()
{
    Game game(mLevelsPath + name().back(), mMenu->trapsEnabled());
    return false;
}
