#include "multiplayermenu.h"

#include "hostmenu.h"
#include "connectmenu.h"

MultiplayerMenu::MultiplayerMenu(const std::string &levelsPath)
    :Menu("Multiplayer"), mLevelsPath(levelsPath)
{
    addItem(new HostButton("Host", levelsPath));
    addItem(new ConnectButton("Connect", levelsPath));
    loop();
}

HostButton::HostButton(const char *name, const std::string &levelsPath)
    :Button(name), mLevelsPath(levelsPath)
{

}

bool HostButton::action()
{
    HostMenu m(mLevelsPath);
    return false;
}

ConnectButton::ConnectButton(const char *name, const std::string &levelsPath)
    :Button(name), mLevelsPath(levelsPath)
{

}

bool ConnectButton::action()
{
    ConnectMenu m(mLevelsPath);
    return false;
}
