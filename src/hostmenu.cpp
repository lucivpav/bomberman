#include "hostmenu.h"

#include "game.h"

#include <fstream>
#include <cassert>

HostMenu::HostMenu(const std::string &levelsPath)
    :Menu("Host"), mLevelsPath(levelsPath)
{
    /* levels */
    addItem( mLevelList = new List("Level") );
    for ( int i = 1 ; i <= 10 ; i++ )
    {
        std::ifstream in(mLevelsPath + std::to_string(i));
        if ( !in )
            break;
        mLevelList->addItem(std::to_string(i).c_str());
    }

    /* lives */
    addItem( mLivesList = new List("Lives") );
    for ( int i = 1 ; i <= 99 ; i++ )
        mLivesList->addItem(std::to_string(i).c_str());
    mLivesList->setDefaultItem(2);

    /* traps */
    addItem( mTrapsEnabledList = new List("Traps") );
    mTrapsEnabledList->addItem("enable");
    mTrapsEnabledList->addItem("disable");

    /* ghosts */
    addItem( mGhostsEnabledList = new List("Ghosts"));
    mGhostsEnabledList->addItem("enable");
    mGhostsEnabledList->addItem("disable");

    addItem( mPortField = new InputField("Port", "88888", 7) );
    addItem( new OkButton("Confirm", *this));
    loop();
}

bool HostMenu::hostInfo(std::string &levelsPath,
                        std::string &level,
                        int &lives,
                        bool &trapsEnabled,
                        bool &ghostsEnabled,
                        std::string &port) const
{
    levelsPath = mLevelsPath;
    level = mLevelList->curItem();

    try
    {
        lives = std::stoi( mLivesList->curItem() );
    }
    catch ( ... )
    {
        assert ( false );
    }

    trapsEnabled = mTrapsEnabledList->curItem() == "enable";
    ghostsEnabled = mGhostsEnabledList->curItem() == "enable";

    port = mPortField->content();

    return true;
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
    int lives;
    bool trapsEnabled;
    bool ghostsEnabled;
    std::string port;

    if ( !mHostMenu.hostInfo(levelsPath, level, lives,
                             trapsEnabled, ghostsEnabled, port) )
    {
        UI::Notification("Invalid setup. Try again");
        return false;
    }

    Game game(levelsPath + level, trapsEnabled, ghostsEnabled,
              lives, "localhost", port.c_str());
    return false;
}
