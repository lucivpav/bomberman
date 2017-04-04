#include "hostmenu.h"

#include "game.h"

#include <fstream>
#include <cassert>

HostMenu::HostMenu(const std::string &levelsPath)
    :Menu("Host"), mLevelsPath(levelsPath)
{
    /* levels */
    addItem( mLevelList = new UI::List("Level") );
    for ( int i = 1 ; i <= 10 ; i++ )
    {
        std::ifstream in(mLevelsPath + std::to_string(i));
        if ( !in )
            break;
        mLevelList->addItem(std::to_string(i).c_str());
    }

    /* lives */
    addItem( mLivesList = new UI::List("Lives") );
    for ( int i = 1 ; i <= 99 ; i++ )
        mLivesList->addItem(std::to_string(i).c_str());
    mLivesList->setDefaultItem(2);

    /* traps */
    addItem( mTrapsEnabledList = new UI::List("Traps") );
    mTrapsEnabledList->addItem("enable");
    mTrapsEnabledList->addItem("disable");

    /* ghosts */
    addItem( mGhostsEnabledList = new UI::List("Ghosts"));
    mGhostsEnabledList->addItem("enable");
    mGhostsEnabledList->addItem("disable");

    addItem( mPortField = new UI::InputField("Port", "39756", 5) );

    addItem( new UI::Button("Confirm",
                        std::bind(&HostMenu::confirmAction, this)) );
    loop();
}

bool HostMenu::confirmAction()
{
    std::string level = mLevelList->curItem();
    int lives;

    try
    {
        lives = std::stoi( mLivesList->curItem() );
    }
    catch ( ... )
    {
        assert ( false );
    }

    bool trapsEnabled = mTrapsEnabledList->curItem() == "enable";
    bool ghostsEnabled = mGhostsEnabledList->curItem() == "enable";

    std::string port = mPortField->content();

    if ( !validPort(port) )
    {
        UI::Notification("Error: Invalid port number");
        return false;
    }

    Game game(mLevelsPath + level, trapsEnabled, ghostsEnabled,
              lives, "localhost", port.c_str());

    return false;
}

bool HostMenu::validPort(const std::string &port)
{
    try
    {
        int p = std::stoi(port);
        if ( p < 1 || p > 65535 )
            throw 42;
    }
    catch ( ... )
    {
        return false;
    }

    return true;
}
