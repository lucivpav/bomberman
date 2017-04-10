#include "mainmenu.h"

#include "singleplayermenu.h"
#include "multiplayermenu.h"
#include "editor.h"

MainMenu::MainMenu(const char * argv0)
    :Menu("Bomberman")
{
    parseLevelsPath(argv0);

    addItem(new UI::Button("Singleplayer",
                       std::bind(&MainMenu::singleplayerAction, this)) );
    addItem(new UI::Button("Multiplayer",
                       std::bind(&MainMenu::multiplayerAction, this)) );
    if ( UI::mGUIMode )
      addItem(new UI::Button("Editor",
                         std::bind(&MainMenu::editorAction, this)) );

    addItem(new UI::Button("Exit",
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

bool MainMenu::editorAction()
{
    Editor editor(mLevelsPath);
    return false;
}

bool MainMenu::exitAction()
{
    return true;
}

void MainMenu::parseLevelsPath(const char *argv0)
{
    std::string tmp(argv0);
    mLevelsPath = tmp.substr(0, tmp.rfind("bomberman")) + "levels/";
}
