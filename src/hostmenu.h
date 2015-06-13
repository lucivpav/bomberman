#ifndef HOSTMENU_H
#define HOSTMENU_H

#include "ui.h"

using namespace UI;

class HostMenu : public Menu
{
public:
    HostMenu(const std::string & levelsPath);
    virtual ~HostMenu() = default;
private:
    std::string mLevelsPath;
    List * mLevelList;
    List * mLivesList;
    List * mTrapsEnabledList;
    List * mGhostsEnabledList;
    InputField * mPortField;

    bool confirmAction();
};


#endif
