#ifndef SINGLEPLAYERMENU_H
#define SINGLEPLAYERMENU_H

#include "ui.h"

using namespace UI;

/**
 * @brief The SingleplayerMenu class represents a Singleplayer menu.
 */
class SingleplayerMenu : public Menu
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
    List * mLevelList;
    List * mLivesList;
    List * mTrapsEnabledList;
    List * mGhostsEnabledList;

    bool confirmAction();
};

#endif
