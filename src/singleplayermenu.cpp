#include "singleplayermenu.h"

#include "game.h"

#include <fstream>

SingleplayerMenu::SingleplayerMenu(const std::string & levelsPath)
    :Menu("Singleplayer"),
      mLevelsPath(levelsPath)
{
    addItem( mLevelList = new List("Level") );
    for ( int i = 1 ; i <= 10 ; i++ )
    {
        std::ifstream in(mLevelsPath + std::to_string(i));
        if ( !in )
            break;
        mLevelList->addItem(std::to_string(i).c_str());
    }

    addItem( mTrapsEnabledList = new List("Traps"));
    mTrapsEnabledList->addItem("enable");
    mTrapsEnabledList->addItem("disable");

    addItem( new OkButton("Confirm", *this));

    loop();
}

void SingleplayerMenu::getInfo(std::string &levelsPath,
                               std::string &level,
                               bool &trapsEnabled) const
{
    levelsPath = mLevelsPath;
    level = mLevelList->curItem();
    trapsEnabled = mTrapsEnabledList->curItem() == "enable";
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
    bool trapsEnabled;
    mMenu.getInfo(levelsPath, level, trapsEnabled);
    Game game(levelsPath + level, trapsEnabled);
    return false;
}
