#ifndef MAINMENU_H
#define MAINMENU_H

#include "ui.h"

using namespace UI;

class MainMenu : public Menu
{
public:
    MainMenu(const char * argv0);
    virtual ~MainMenu() = default;
private:
    std::string mLevelsPath;

    bool singleplayerAction();
    bool multiplayerAction();
    bool exitAction();

    void parseLevelsPath(const char *argv0);
};

#endif
