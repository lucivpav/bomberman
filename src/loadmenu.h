#ifndef LOADMENU_H
#define LOADMENU_H

#include "ui.h"
#include "map.h"

class LoadMenu : public UI::Menu
{
public:
  LoadMenu(const std::string & levelsPath, Map & map);
  virtual ~LoadMenu() = default;
private:
  UI::InputField * mName;
  const std::string & mLevelsPath;
  Map & mMap;
  bool confirmAction();
};

#endif // LOADMENU_H
