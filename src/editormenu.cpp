#include "editormenu.h"

#include "editor.h"
#include "savemenu.h"
#include "loadmenu.h"

EditorMenu::EditorMenu(const std::string &levelsPath, bool & exit,
                       Map &map)
  :Menu("Editor"),
    mLevelsPath(levelsPath),
    mExit(exit),
    mMap(map)
{
  addItem( new UI::Button("Save",
                          std::bind(&EditorMenu::saveAction, this)) );
  addItem( new UI::Button("Load",
                          std::bind(&EditorMenu::loadAction, this)) );
  addItem( new UI::Button("Exit",
                          std::bind(&EditorMenu::exitAction, this)) );
  loop();
}

bool EditorMenu::saveAction()
{
  SaveMenu menu(mLevelsPath, mMap);
  return false;
}

bool EditorMenu::loadAction()
{
  LoadMenu menu(mLevelsPath, mMap);
  return true;
}

bool EditorMenu::exitAction()
{
  mExit = true;
  return true;
}
