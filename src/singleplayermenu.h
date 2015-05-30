#ifndef SINGLEPLAYERMENU_H
#define SINGLEPLAYERMENU_H

#include "ui.h"

using namespace UI;

class SingleplayerMenu : public Menu
{
public:
    SingleplayerMenu(const std::string & levelsPath);
    virtual ~SingleplayerMenu() = default;

    void getInfo(std::string & levelsPath,
                 std::string & level,
                 bool & trapsEnabled) const;
private:
    std::string mLevelsPath;
    List * mLevelList;
    List * mTrapsEnabledList;

    class OkButton : public Button
    {
    public:
        OkButton(const char * name,
                 const SingleplayerMenu & menu);
        virtual ~OkButton() = default;

        virtual bool action();
    private:
        const SingleplayerMenu & mMenu;
    };
};

#endif
