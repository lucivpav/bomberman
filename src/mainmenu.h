#ifndef MAINMENU_H
#define MAINMENU_H

#include "ui.h"

/**
 * @brief The MainMenu class represents a main menu.
 */
class MainMenu : public UI::Menu
{
public:
    /**
     * @param argv0 The argv[0] parameter that main() receives.
     */
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
