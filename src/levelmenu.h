#ifndef LEVELMENU_H
#define LEVELMENU_H

#include "menu.h"

class LevelMenu : public Menu
{
public:
    LevelMenu(const std::string & argv0);
    void keyEvent(int key);
    void drawEvent();
    bool trapsEnabled() const;
private:
    std::string mArgv0;
    bool mTrapsEnabled;
};

class LevelItem : public MenuItem
{
public:
    LevelItem(const char * name,
              const std::string & levelsPath,
              const LevelMenu & menu);
    bool action();
private:
    static std::string mLevelsPath;
    static const LevelMenu * mMenu;
};

#endif
