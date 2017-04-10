#include "loadmenu.h"

LoadMenu::LoadMenu(const std::string &levelsPath, Map & map)
  :Menu("Load"),
    mLevelsPath(levelsPath),
    mMap(map)
{
  mName = new UI::InputField("Name", "", 30);
  addItem(mName);

  addItem( new UI::Button("Confirm",
                          std::bind(&LoadMenu::confirmAction, this)) );
  loop();
}

bool LoadMenu::confirmAction()
{
  std::string fileName = mName->content();
  if ( fileName.empty() ) {
    UI::Notification n("Invalid level name");
    return false;
  }
  mMap.load_keep((mLevelsPath + fileName).c_str());
  return true;
}
