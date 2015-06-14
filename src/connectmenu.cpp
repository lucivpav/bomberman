#include "connectmenu.h"
#include "clientgame.h"

ConnectMenu::ConnectMenu(const std::string &levelsPath)
    :Menu("Connect"), mLevelsPath(levelsPath)
{
    addItem( mAddressField = new InputField("Address", "localhost", 40) );
    addItem( mPortField = new InputField("Port", "39756", 5) );
    addItem(new Button("Confirm",
                         std::bind(&ConnectMenu::confirmAction, this)));

    loop();
}

bool ConnectMenu::confirmAction()
{
    std::string address = mAddressField->content();
    std::string port = mPortField->content();

    if ( !validAddress(address) )
    {
        UI::Notification("Error: Invalid address");
        return false;
    }

    if ( !validPort(port) )
    {
        UI::Notification("Error: Invalid port number");
        return false;
    }

    ClientGame game(address.c_str(), port.c_str());
    return false;
}

bool ConnectMenu::validAddress(const std::string &address)
{
    if ( address.empty() )
        return false;
    return true;
}

bool ConnectMenu::validPort(const std::string &port)
{
    try
    {
        int p = std::stoi(port);
        if ( p < 1 || p > 65535 )
            throw 42;
    }
    catch ( ... )
    {
        return false;
    }

    return true;
}
