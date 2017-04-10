#include "savemenu.h"

SaveMenu::SaveMenu(const std::string & levelsPath, const Map &map)
  :Menu("Save"),
    mLevelsPath(levelsPath),
    mMap(map)
{
  mName = new UI::InputField("Name", "", 30);
  addItem(mName);

  addItem( new UI::Button("Confirm",
                          std::bind(&SaveMenu::confirmAction, this)) );
  loop();
}

bool SaveMenu::confirmAction()
{
  std::string fileName = mName->content();
  if ( fileName.empty() ) {
    UI::Notification n("Invalid level name");
    return false;
  }
  mMap.save(mLevelsPath + fileName);
  return true;
}
