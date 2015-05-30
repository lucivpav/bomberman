#include "connectmenu.h"

ConnectMenu::ConnectMenu(const std::string &levelsPath)
    :Menu("Connect"), mLevelsPath(levelsPath)
{
    addItem( mAddressField = new InputField("Address", "loopback", 40) );
    addItem( mPortField = new InputField("Port", "8080", 7) );
    addItem(new OkButton("Confirm", *this));
    loop();
}


void ConnectMenu::connectInfo(std::string &address, int &port) const
{
    address = mAddressField->content();
    port = std::stoi(mPortField->content());
}


ConnectMenu::OkButton::OkButton(const char *name, const ConnectMenu &connectMenu)
    :Button(name), mConnectMenu(connectMenu)
{

}


bool ConnectMenu::OkButton::action()
{
    std::string address;
    int port;
    mConnectMenu.connectInfo(address, port);
    // todo
    return false;
}
