#ifndef GAMEGUI_H
#define GAMEGUI_H

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "block.h"
#include "player.h"
#include "enemy.h"

class GameGUI
{
private:
  enum Direction {FRONT, BACK, LEFT, RIGHT, N_DIRECTIONS};
private:
  SDL_Texture * mBlockTextures[Block::N_TYPES];
  SDL_Texture * mPlayerTextures[N_DIRECTIONS];
  SDL_Texture * mEnemyTextures[N_DIRECTIONS];
  SDL_Texture * mPlayerStatusTexture;
  SDL_Texture * mEnemyStatusTexture;

  struct Status {
    int lives;
    int bombs;
  };

  Status mPrevPlayerStatus;
  Status mPrevEnemyStatus;
public:
  GameGUI();
  ~GameGUI();
  void drawBlock(Block::Type t, int col, int row) const;
  void drawPlayer(const Player & p) const;
  void drawEnemy(const Enemy & p) const;
  void drawTexture(SDL_Texture * t, int col, int row) const;

  SDL_Texture * getStatusTexture(bool enemy, int lives, int bombs);
private:
  SDL_Texture *loadTexture(const std::string &path);
};

#endif // GAMEGUI_H
