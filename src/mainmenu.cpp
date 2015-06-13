#include "mainmenu.h"

#include "singleplayermenu.h"
#include "multiplayermenu.h"

MainMenu::MainMenu(const char * argv0)
    :Menu("Bomberman")
{
    parseLevelsPath(argv0);

    addItem(new Button("Singleplayer",
                       std::bind(&MainMenu::singleplayerAction, this)) );
    addItem(new Button("Multiplayer",
                       std::bind(&MainMenu::multiplayerAction, this)) );
    addItem(new Button("Exit",
                       std::bind(&MainMenu::exitAction, this)) );

    loop();
}

bool MainMenu::singleplayerAction()
{
    SingleplayerMenu menu(mLevelsPath);
    return false;
}

bool MainMenu::multiplayerAction()
{
    MultiplayerMenu menu(mLevelsPath);
    return false;
}

bool MainMenu::exitAction()
{
    return true;
}

void MainMenu::parseLevelsPath(const char *argv0)
{
    /*
         * gcc - FAIL
         * ./gcc
         * ope/asf/gcc
         * ../.././../asf/ff/gcc
         * ../gcc
         */
    std::string tmp(argv0);
    mLevelsPath = tmp.substr(0, tmp.rfind("bomberman")) + "levels/";
}
