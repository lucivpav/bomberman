#include "singleplayermenu.h"

#include "game.h"

#include <fstream>
#include <string>
#include <cassert>

SingleplayerMenu::SingleplayerMenu(const std::string & levelsPath)
    :Menu("Singleplayer"),
      mLevelsPath(levelsPath)
{
    /* levels */
    addItem( mLevelList = new UI::List("Level") );
    for ( int i = 1 ; i <= 10 ; i++ )
    {
        std::ifstream in(mLevelsPath + std::to_string(i));
        if ( !in )
        {
            if ( i == 1 )
                UI::Notification("Error: Failed to load levels");
            break;
        }
        mLevelList->addItem(std::to_string(i).c_str());
    }

    /* lives */
    addItem( mLivesList = new UI::List("Lives") );
    for ( int i = 1 ; i <= 99 ; i++ )
        mLivesList->addItem(std::to_string(i).c_str());
    mLivesList->setDefaultItem(2);

    /* traps */
    addItem( mTrapsEnabledList = new UI::List("Traps"));
    mTrapsEnabledList->addItem("enable");
    mTrapsEnabledList->addItem("disable");

    /* ghosts */
    addItem( mGhostsEnabledList = new UI::List("Ghosts"));
    mGhostsEnabledList->addItem("enable");
    mGhostsEnabledList->addItem("disable");

    /* ok button */
    addItem( new UI::Button("Confirm",
                        std::bind(&SingleplayerMenu::confirmAction, this)) );

    loop();
}

bool SingleplayerMenu::confirmAction()
{
    std::string level = mLevelList->curItem();
    int lives;

    try
    {
        lives = std::stoi( mLivesList->curItem() );
    }
    catch ( ... )
    {
        assert ( false ); // this should not be possible
    }

    bool trapsEnabled = mTrapsEnabledList->curItem() == "enable";
    bool ghostsEnabled = mGhostsEnabledList->curItem() == "enable";

    Game(mLevelsPath + level, trapsEnabled, ghostsEnabled, lives);
    return false;
}
