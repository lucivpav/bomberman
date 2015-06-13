#ifndef HOSTMENU_H
#define HOSTMENU_H

#include "ui.h"

using namespace UI;

/**
 * @brief The HostMenu class represents a Host menu.
 */
class HostMenu : public Menu
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
    List * mLevelList;
    List * mLivesList;
    List * mTrapsEnabledList;
    List * mGhostsEnabledList;
    InputField * mPortField;

    bool confirmAction();
};


#endif
