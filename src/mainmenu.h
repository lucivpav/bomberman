#ifndef MAINMENU_H
#define MAINMENU_H

#include "menu.h"

class SingleplayerItem : public MenuItem
{
public:
    SingleplayerItem(const char * name, const char *argv0);
    bool action();
private:
    const char * mArgv0;
};

class MultiplayerItem : public MenuItem
{
public:
    MultiplayerItem(const char * name);
    bool action();
};

class ExitItem : public MenuItem
{
public:
    ExitItem(const char * name);
    bool action();
};

class MainMenu : public Menu
{
public:
    MainMenu(const char * argv0);
    ~MainMenu();
};

#endif
