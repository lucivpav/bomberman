#ifndef MULTIPLAYERMENU_H
#define MULTIPLAYERMENU_H

#include "ui.h"

using namespace UI;

class MultiplayerMenu : public Menu
{
public:
    MultiplayerMenu(const std::string & levelsPath);
    virtual ~MultiplayerMenu() = default;
private:
    std::string mLevelsPath;

    bool hostAction();
    bool connectAction();
};

#endif
