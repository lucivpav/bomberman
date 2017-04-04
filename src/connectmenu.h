#ifndef CONNECTMENU_H
#define CONNECTMENU_H

#include "ui.h"

/**
 * @brief The ConnectMenu class represents a Connect menu.
 */
class ConnectMenu : public UI::Menu
{
public:
    /**
     * @param levelsPath The file system path to folder where
     * levels are located.
     */
    ConnectMenu(const std::string & levelsPath);

    virtual ~ConnectMenu() = default;
private:
    std::string mLevelsPath;
    UI::InputField * mAddressField;
    UI::InputField * mPortField;

    bool confirmAction();

    bool validAddress(const std::string & address);
    bool validPort(const std::string & port);
};

#endif
