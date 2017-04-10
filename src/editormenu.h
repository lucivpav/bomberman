#ifndef EDITORMENU_H
#define EDITORMENU_H

#include "ui.h"
#include "map.h"

class EditorMenu : public UI::Menu
{
public:
  EditorMenu(const std::string & levelsPath, bool &exit, Map & map);
  virtual ~EditorMenu() = default;
private:
  std::string mLevelsPath;

  bool saveAction();
  bool loadAction();
  bool exitAction();

  bool & mExit;
  Map & mMap;
};

#endif // EDITORMENU_H
