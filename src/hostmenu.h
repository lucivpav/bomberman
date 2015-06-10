#ifndef HOSTMENU_H
#define HOSTMENU_H

#include "ui.h"

using namespace UI;

class HostMenu : public Menu
{
public:
    HostMenu(const std::string & levelsPath);
    virtual ~HostMenu() = default;

    void hostInfo(std::string & levelsPath,
                  std::string & level,
                  bool & trapsEnabled,
                  std::string &port) const;
private:
    std::string mLevelsPath;
    List * mLevelList;
    List * mTrapsEnabledList;
    InputField * mPortField;

    class OkButton : public Button
    {
    public:
        OkButton(const char * name,
                 const HostMenu & hostMenu);
        virtual ~OkButton() = default;

        virtual bool action();
    private:
        const HostMenu & mHostMenu;
    };
};


#endif
