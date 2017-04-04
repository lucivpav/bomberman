#ifndef SINGLEPLAYERMENU_H
#define SINGLEPLAYERMENU_H

#include "ui.h"

/**
 * @brief The SingleplayerMenu class represents a Singleplayer menu.
 */
class SingleplayerMenu : public UI::Menu
{
public:
    /**
     * @param levelsPath The file system path to folder where
     * levels are located.
     */
    SingleplayerMenu(const std::string & levelsPath);

    virtual ~SingleplayerMenu() = default;
private:
    std::string mLevelsPath;
    UI::List * mLevelList;
    UI::List * mLivesList;
    UI::List * mTrapsEnabledList;
    UI::List * mGhostsEnabledList;

    bool confirmAction();
};

#endif
