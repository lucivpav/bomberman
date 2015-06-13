#include "connectmenu.h"
#include "clientgame.h"

ConnectMenu::ConnectMenu(const std::string &levelsPath)
    :Menu("Connect"), mLevelsPath(levelsPath)
{
    addItem( mAddressField = new InputField("Address", "localhost", 40) );
    addItem( mPortField = new InputField("Port", "88888", 7) );
    addItem(new Button("Confirm",
                         std::bind(&ConnectMenu::confirmAction, this)));

    loop();
}

bool ConnectMenu::confirmAction()
{
    std::string address = mAddressField->content();
    std::string port = mPortField->content();

    ClientGame game(address.c_str(), port.c_str());
    return false;
}
