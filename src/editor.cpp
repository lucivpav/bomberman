#include "editor.h"

#include <set>
#include <thread>
#include <chrono>

#include "ui.h"
#include "editormenu.h"

Editor::Editor(const std::string &levelsPath)
  :mLevelsPath(levelsPath),
    mCurLevel(DEFAULT_LEVEL),
    mEndLoop(false),
    mCurBlock(1)
{
  mPossibleBlocks = { Block::WALL, Block::NICE_WALL,
                      Block::PLAYER, Block::ENEMY, Block::TRAP_OPENED,
                      Block::DESTRUCTABLE };
  Pos player, enemy;
  std::set<Pos> traps;
  std::string file = levelsPath + DEFAULT_LEVEL;
  mMap.load(file.c_str(), player, enemy, traps, &mGameGUI, true);
  loop();
}

void Editor::loop()
{
  while (!mEndLoop) {
    keyEvent();
    drawEvent();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000/30));
  }
}

void Editor::keyEvent()
{
  SDL_Event e;
  while ( SDL_PollEvent(&e) ) {
    switch (e.type) {
    case SDL_QUIT: exitAction(); return;
    case SDL_KEYDOWN:{
      if ( e.key.keysym.sym == 'q' )
        exitAction();} break;
    case SDL_MOUSEWHEEL:
      pickBlock(e.wheel.y); break;
    case SDL_MOUSEBUTTONDOWN:
      if ( e.button.button == SDL_BUTTON_LEFT ) setBlock();
      if ( e.button.button == SDL_BUTTON_RIGHT ) setBlock(true);
      break;
    }
  }
}

void Editor::drawEvent()
{
  SDL_SetRenderDrawColor(UI::mGUI.renderer, 76, 65, 65, 0xFF);
  SDL_RenderClear(UI::mGUI.renderer);
  drawGrid();
  mMap.drawGUI();
  drawPickedBlock();
  SDL_RenderPresent(UI::mGUI.renderer);
}

void Editor::drawGrid()
{
  SDL_SetRenderDrawColor( UI::mGUI.renderer, 0, 0, 0, 0xFF );
  int bsize = UI::mGUI.BLOCK_SIZE;
  for ( int col = 0 ; col < UI::mGUI.WINDOW_WIDTH ; ++col )
  {
    int x_from = col * bsize;
    int y_from = 0;
    int x_to = x_from;
    int y_to = UI::mGUI.WINDOW_HEIGHT-1-bsize;
    SDL_RenderDrawLine(UI::mGUI.renderer, x_from, y_from, x_to, y_to);
  }

  for ( int row = 0 ; row < UI::mGUI.WINDOW_HEIGHT-1; ++row )
  {
    int x_from = 0;
    int y_from = row * bsize;
    int x_to = UI::mGUI.WINDOW_WIDTH-1;
    int y_to = y_from;
    SDL_RenderDrawLine(UI::mGUI.renderer, x_from, y_from, x_to, y_to);
  }
}

void Editor::drawPickedBlock()
{
  int col, row;
  getCurBlockCoordinates(col, row);
  if ( row == Map::maxHeight ) return;
  mGameGUI.drawBlock(mPossibleBlocks[mCurBlock], col, row);
}

void Editor::pickBlock(int dir)
{
  mCurBlock += dir;
  if ( mCurBlock < 0 ) mCurBlock = mPossibleBlocks.size()-1;
  mCurBlock %= mPossibleBlocks.size();
}

void Editor::setBlock(bool empty)
{
  int col, row;
  getCurBlockCoordinates(col, row);
  Block::Type block = empty ? Block::EMPTY : mPossibleBlocks[mCurBlock];
  mMap.set(Pos(col,row), block);
}

void Editor::getCurBlockCoordinates(int &col, int &row)
{
  int bsize = UI::mGUI.BLOCK_SIZE;
  SDL_GetMouseState(&col, &row);
  col /= bsize;
  row /= bsize;
}

void Editor::exitAction()
{
  EditorMenu menu(mLevelsPath, mEndLoop, mMap);
}
