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

    addItem( new Button("Confirm",
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

    Game game(mLevelsPath + level, trapsEnabled, ghostsEnabled,
              lives, "localhost", port.c_str());

    return false;
}
