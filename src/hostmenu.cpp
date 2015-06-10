#include "hostmenu.h"

#include "game.h"

#include <fstream>

HostMenu::HostMenu(const std::string &levelsPath)
    :Menu("Host"), mLevelsPath(levelsPath)
{
    addItem( mLevelList = new List("Level") );
    for ( int i = 1 ; i <= 10 ; i++ )
    {
        std::ifstream in(mLevelsPath + std::to_string(i));
        if ( !in )
            break;
        mLevelList->addItem(std::to_string(i).c_str());
    }

    addItem( mTrapsEnabledList = new List("Traps") );
    mTrapsEnabledList->addItem("enable");
    mTrapsEnabledList->addItem("disable");

    addItem( mPortField = new InputField("Port", "88888", 7) );
    addItem( new OkButton("Confirm", *this));
    loop();
}

void HostMenu::hostInfo(std::string &levelsPath,
                        std::string &level,
                        bool &trapsEnabled,
                        std::string &port) const
{
    levelsPath = mLevelsPath;
    level = mLevelList->curItem();
    trapsEnabled = mTrapsEnabledList->curItem() == "enable";
    port = mPortField->content();
}


HostMenu::OkButton::OkButton(const char *name,
                             const HostMenu &hostMenu)
    :Button(name), mHostMenu(hostMenu)
{

}

bool HostMenu::OkButton::action()
{
    std::string levelsPath;
    std::string level;
    bool trapsEnabled;
    std::string port;
    mHostMenu.hostInfo(levelsPath, level, trapsEnabled, port);
    Game game(levelsPath + level, trapsEnabled, "localhost", port.c_str());
    return false;
}
