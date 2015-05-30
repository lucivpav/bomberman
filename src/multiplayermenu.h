#ifndef MULTIPLAYERMENU_H
#define MULTIPLAYERMENU_H

#include "ui.h"

using namespace UI;

class MultiplayerMenu : public Menu
{
public:
    MultiplayerMenu(const std::string & levelsPath);
    virtual ~MultiplayerMenu() = default;
private:
    std::string mLevelsPath;
};

class HostButton : public Button
{
public:
    HostButton(const char * name,
               const std::string & levelsPath);
    virtual ~HostButton() = default;

    virtual bool action();
private:
    std::string mLevelsPath;
};

class ConnectButton : public Button
{
public:
    ConnectButton(const char * name,
                  const std::string & levelsPath);
    virtual ~ConnectButton() = default;

    virtual bool action();
private:
    std::string mLevelsPath;
};
#endif
