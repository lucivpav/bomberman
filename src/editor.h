#ifndef EDITOR_H
#define EDITOR_H

#include <string>
#include <vector>
#include "map.h"
#include "game_gui.h"
#include "block.h"

class Editor {
public:
  const std::string DEFAULT_LEVEL = "editor_default";
  Editor(const std::string & levelsPath);
private:
  void loop();
  void keyEvent();
  void drawEvent();
  void drawGrid();
  void drawPickedBlock();
  void pickBlock(int dir);
  void setBlock(bool empty = false);
  void getCurBlockCoordinates(int & col, int & row);

  void exitAction();

  std::string mLevelsPath;
  std::string mCurLevel;
  bool mEndLoop;
  Map mMap;
  GameGUI mGameGUI;

  std::vector<Block::Type> mPossibleBlocks;
  int mCurBlock;
};

#endif // EDITOR_H
