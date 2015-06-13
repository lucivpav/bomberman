#ifndef SINGLEPLAYERMENU_H
#define SINGLEPLAYERMENU_H

#include "ui.h"

using namespace UI;

class SingleplayerMenu : public Menu
{
public:
    SingleplayerMenu(const std::string & levelsPath);
    virtual ~SingleplayerMenu() = default;

    bool getInfo(std::string & levelsPath,
                 std::string & level,
                 int & lives,
                 bool & trapsEnabled,
                 bool & ghostsEnabled) const;
private:
    std::string mLevelsPath;
    List * mLevelList;
    List * mLivesList;
    List * mTrapsEnabledList;
    List * mGhostsEnabledList;

    bool confirmAction();
};

#endif
