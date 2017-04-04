#include "multiplayermenu.h"

#include "hostmenu.h"
#include "connectmenu.h"

MultiplayerMenu::MultiplayerMenu(const std::string &levelsPath)
    :Menu("Multiplayer"), mLevelsPath(levelsPath)
{
    addItem(new UI::Button("Host",
                           std::bind(&MultiplayerMenu::hostAction, this)));
    addItem(new UI::Button("Connect",
                           std::bind(&MultiplayerMenu::connectAction, this)));
    loop();
}

bool MultiplayerMenu::hostAction()
{
    HostMenu m(mLevelsPath);
    return false;
}

bool MultiplayerMenu::connectAction()
{
    ConnectMenu m(mLevelsPath);
    return false;
}
