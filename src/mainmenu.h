#ifndef MAINMENU_H
#define MAINMENU_H

#include "menu.h"

class SingleplayerItem : public MenuItem
{
public:
    SingleplayerItem(const char * name);
    bool action();
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

class MainMenu
{
public:
    MainMenu();
    ~MainMenu();
private:
    Menu menu;
};

#endif
