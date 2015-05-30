#include "mainmenu.h"

#include "singleplayermenu.h"
#include "multiplayermenu.h"

SingleplayerButton::SingleplayerButton(const char * name,
                                       const std::string &levelsPath)
    :Button(name), mLevelsPath(levelsPath)
{
}

bool SingleplayerButton::action()
{
    SingleplayerMenu menu(mLevelsPath);
    return false;
}


MultiplayerButton::MultiplayerButton(const char * name,
                                     const std::string & levelsPath)
    :Button(name), mLevelsPath(levelsPath)
{
}

bool MultiplayerButton::action()
{
    MultiplayerMenu menu(mLevelsPath);
    return false;
}


ExitButton::ExitButton(const char * name)
    :Button(name)
{
}

bool ExitButton::action()
{
    return true;
}

MainMenu::MainMenu(const char * argv0)
    :Menu("Bomberman")
{
    parseLevelsPath(argv0);

    addItem(new SingleplayerButton("Singleplayer", mLevelsPath));
    addItem(new MultiplayerButton("Multiplayer", mLevelsPath));
    addItem(new ExitButton("Exit"));

    loop();
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
