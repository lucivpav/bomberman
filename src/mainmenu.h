#ifndef MAINMENU_H
#define MAINMENU_H

#include "ui.h"

using namespace UI;

class SingleplayerButton : public Button
{
public:
    SingleplayerButton(const char * name,
                       const std::string & levelsPath);
    virtual ~SingleplayerButton() = default;

    virtual bool action();
private:
    std::string mLevelsPath;
};

class MultiplayerButton : public Button
{
public:
    MultiplayerButton(const char * name,
                      const std::string & levelsPath);
    virtual ~MultiplayerButton() = default;

    virtual bool action();
private:
    std::string mLevelsPath;
};

class ExitButton : public Button
{
public:
    ExitButton(const char * name);
    virtual ~ExitButton() = default;

    virtual bool action();
};

class MainMenu : public Menu
{
public:
    MainMenu(const char * argv0);
    virtual ~MainMenu() = default;
private:
    std::string mLevelsPath;

    void parseLevelsPath(const char * argv0);
};

#endif
