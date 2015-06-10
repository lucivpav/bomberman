#include "connectmenu.h"
#include "clientgame.h"

ConnectMenu::ConnectMenu(const std::string &levelsPath)
    :Menu("Connect"), mLevelsPath(levelsPath)
{
    addItem( mAddressField = new InputField("Address", "localhost", 40) );
    addItem( mPortField = new InputField("Port", "88888", 7) );
    addItem(new OkButton("Confirm", *this));

    loop();
}

void ConnectMenu::connectInfo(std::string &address, std::string &port) const
{
    address = mAddressField->content();
    port = mPortField->content();
}

ConnectMenu::OkButton::OkButton(const char *name, const ConnectMenu &connectMenu)
    :Button(name), mConnectMenu(connectMenu)
{

}

bool ConnectMenu::OkButton::action()
{
    std::string address, port;
    mConnectMenu.connectInfo(address, port);
    ClientGame game(address.c_str(), port.c_str());
    return false;
}
