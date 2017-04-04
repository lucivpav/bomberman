#ifndef MULTIPLAYERMENU_H
#define MULTIPLAYERMENU_H

#include "ui.h"

/**
 * @brief The MultiplayerMenu class represents a Multiplayer menu.
 */
class MultiplayerMenu : public UI::Menu
{
public:
    /**
     * @param levelsPath The file system path to folder where
     * levels are located.
     */
    MultiplayerMenu(const std::string & levelsPath);

    virtual ~MultiplayerMenu() = default;
private:
    std::string mLevelsPath;

    bool hostAction();
    bool connectAction();
};

#endif
