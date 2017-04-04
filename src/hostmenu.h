#ifndef HOSTMENU_H
#define HOSTMENU_H

#include "ui.h"

/**
 * @brief The HostMenu class represents a Host menu.
 */
class HostMenu : public UI::Menu
{
public:
    /**
     * @param levelsPath The file system path to folder where
     * levels are located.
     */
    HostMenu(const std::string & levelsPath);

    virtual ~HostMenu() = default;
private:
    std::string mLevelsPath;
    UI::List * mLevelList;
    UI::List * mLivesList;
    UI::List * mTrapsEnabledList;
    UI::List * mGhostsEnabledList;
    UI::InputField * mPortField;

    bool confirmAction();
    bool validPort(const std::string & port);
};


#endif
