#ifndef CONNECTMENU_H
#define CONNECTMENU_H

#include "ui.h"

using namespace UI;

class ConnectMenu : public Menu
{
public:
    ConnectMenu(const std::string & levelsPath);
    virtual ~ConnectMenu() = default;

    void connectInfo(std::string & address,
                     std::string &port) const;
private:
    std::string mLevelsPath;
    InputField * mAddressField;
    InputField * mPortField;

    bool confirmAction();
};

#endif
