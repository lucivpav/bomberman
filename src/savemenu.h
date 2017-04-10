#ifndef SAVEMENU_H
#define SAVEMENU_H

#include "ui.h"
#include "map.h"

class SaveMenu : public UI::Menu
{
public:
  SaveMenu(const std::string & levelsPath, const Map & map);
  virtual ~SaveMenu() = default;
private:
  UI::InputField * mName;
  const std::string & mLevelsPath;
  const Map & mMap;
  bool confirmAction();
};

#endif // SAVEMENU_H
