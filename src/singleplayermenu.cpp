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
    addItem( mTrapsEnabledList = new List("Traps"));
    mTrapsEnabledList->addItem("enable");
    mTrapsEnabledList->addItem("disable");

    /* ghosts */
    addItem( mGhostsEnabledList = new List("Ghosts"));
    mGhostsEnabledList->addItem("enable");
    mGhostsEnabledList->addItem("disable");

    /* ok button */
    addItem( new OkButton("Confirm", *this));

    loop();
}

bool SingleplayerMenu::getInfo(std::string &levelsPath,
                               std::string &level,
                               int &lives,
                               bool &trapsEnabled,
                               bool &ghostsEnabled) const
{
    levelsPath = mLevelsPath;
    level = mLevelList->curItem();

    try
    {
        lives = std::stoi( mLivesList->curItem() );
    }
    catch ( ... )
    {
        assert ( false ); // this should not be possible
    }

    trapsEnabled = mTrapsEnabledList->curItem() == "enable";
    ghostsEnabled = mGhostsEnabledList->curItem() == "enable";

    return true;
}

SingleplayerMenu::OkButton::OkButton(const char *name,
                                     const SingleplayerMenu & menu)
    :Button(name), mMenu(menu)
{

}

bool SingleplayerMenu::OkButton::action()
{
    std::string levelsPath;
    std::string level;
    int lives;
    bool trapsEnabled;
    bool ghostsEnabled;

    if ( !mMenu.getInfo(levelsPath, level, lives, trapsEnabled, ghostsEnabled) )
    {
        UI::Notification("Invalid setup. Try again");
        return false;
    }

    Game(levelsPath + level, trapsEnabled, ghostsEnabled, lives);
    return false;
}
