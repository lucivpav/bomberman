#include "game_gui.h"

#include <cassert>

#include "ui.h"

GameGUI::GameGUI()
   :mPlayerStatusTexture(nullptr),
    mEnemyStatusTexture(nullptr),
    mPrevPlayerStatus({-1,-1}),
    mPrevEnemyStatus({-1,-1})
{
  for ( int i = 0 ; i < Block::N_TYPES ; ++i )
    mBlockTextures[i] = nullptr;

  for ( int i = 0 ; i < N_DIRECTIONS ; ++i ) {
    mPlayerTextures[i] = nullptr;
    mEnemyTextures[i] = nullptr;
  }

  mBlockTextures[Block::WALL] = loadTexture("assets/wall.png");
  mBlockTextures[Block::NICE_WALL] = loadTexture("assets/nice_wall.png");
  mBlockTextures[Block::GHOST] = loadTexture("assets/ghost.png");
  mBlockTextures[Block::TRAP_CLOSED] = loadTexture("assets/trap_closed.png");
  mBlockTextures[Block::TRAP_OPENED] = loadTexture("assets/trap_opened.png");
  mBlockTextures[Block::DESTRUCTABLE] = loadTexture("assets/destructable.png");
  mBlockTextures[Block::TIMED_BOMB] = loadTexture("assets/timed_bomb.png");
  mBlockTextures[Block::FLAME] = loadTexture("assets/flame.png");
  mBlockTextures[Block::BONUS_BOMB] = loadTexture("assets/bonus_bomb.png");
  mBlockTextures[Block::BONUS_SPEED] = loadTexture("assets/bonus_speed.png");
  mBlockTextures[Block::BONUS_REMOTE] = loadTexture("assets/bonus_remote.png");
  mBlockTextures[Block::BONUS_RADIUS] = loadTexture("assets/bonus_radius.png");
  mBlockTextures[Block::REMOTE_BOMB] = loadTexture("assets/remote_bomb.png");

  /* player */
  mPlayerTextures[FRONT] = loadTexture("assets/player_front.png");
  mPlayerTextures[BACK] = loadTexture("assets/player_back.png");
  mPlayerTextures[LEFT] = loadTexture("assets/player_left.png");
  mPlayerTextures[RIGHT] = loadTexture("assets/player_right.png");

  /* enemy */
  mEnemyTextures[FRONT] = loadTexture("assets/enemy_front.png");
  mEnemyTextures[BACK] = loadTexture("assets/enemy_back.png");
  mEnemyTextures[LEFT] = loadTexture("assets/enemy_left.png");
  mEnemyTextures[RIGHT] = loadTexture("assets/enemy_right.png");
}

GameGUI::~GameGUI()
{
  for ( int i = 0 ; i < Block::N_TYPES ; ++i )
    SDL_DestroyTexture(mBlockTextures[i]);

  for ( int i = 0 ; i < N_DIRECTIONS ; ++i ) {
    SDL_DestroyTexture(mPlayerTextures[i]);
    SDL_DestroyTexture(mEnemyTextures[i]);
  }
  SDL_DestroyTexture(mPlayerStatusTexture);
  SDL_DestroyTexture(mEnemyStatusTexture);
}

void GameGUI::drawBlock(Block::Type t, int col, int row) const
{
  if ( !mBlockTextures[t] ) return;
  drawTexture(mBlockTextures[t], col, row);
}

void GameGUI::drawPlayer(const Player &p) const
{
  Pos pos = p.getPos();
  Pos dir = p.getDir();
  SDL_Texture * texture = nullptr;
  if ( dir == Pos(1,0) ) texture = mPlayerTextures[RIGHT];
  else if ( dir == Pos(-1,0) ) texture = mPlayerTextures[LEFT];
  else if ( dir == Pos(0,1) ) texture = mPlayerTextures[FRONT];
  else if ( dir == Pos(0,-1) ) texture = mPlayerTextures[BACK];
  else assert ( false );
  drawTexture(texture, pos.x, pos.y);
}

void GameGUI::drawEnemy(const Enemy &p) const
{
  Pos pos = p.getPos();
  Pos dir = p.getDir();
  SDL_Texture * texture = nullptr;
  if ( dir == Pos(1,0) ) texture = mEnemyTextures[RIGHT];
  else if ( dir == Pos(-1,0) ) texture = mEnemyTextures[LEFT];
  else if ( dir == Pos(0,1) ) texture = mEnemyTextures[FRONT];
  else if ( dir == Pos(0,-1) ) texture = mEnemyTextures[BACK];
  else assert ( false );
  drawTexture(texture, pos.x, pos.y);
}

SDL_Texture *GameGUI::loadTexture(const std::string & path)
{
  SDL_Texture* texture = nullptr;

  SDL_Surface* surface = IMG_Load(path.c_str());
  if( !surface )
    UI::mGUI.error(std::string("IMG_Load failed for ") + path
                   + ". " + IMG_GetError());

  texture = SDL_CreateTextureFromSurface(UI::mGUI.renderer, surface);
  if( !texture )
    UI::mGUI.error(std::string("CreateTextureFromSurface failed for ")
                   + path);

  SDL_FreeSurface(surface);
  return texture;
}

void GameGUI::drawTexture(SDL_Texture *texture, int col, int row) const
{
  int bsize = UI::mGUI.BLOCK_SIZE;
  int x = col * bsize;
  int y = row * bsize;

  SDL_Rect qrect = {x,y, 0, 0};
  SDL_QueryTexture(texture,0,0,&qrect.w, &qrect.h);

  int h = double(qrect.h)*double(bsize)/double(qrect.w);
  SDL_Rect rect = {x,y-(h-bsize),bsize,h};
  SDL_RenderCopyEx(UI::mGUI.renderer, texture, 0, &rect,
                   0.0, 0, SDL_FLIP_NONE);
}

SDL_Texture *GameGUI::getStatusTexture(bool enemy, int lives, int bombs)
{
  Status & st = enemy ? mPrevEnemyStatus : mPrevPlayerStatus;
  SDL_Texture * t = enemy ? mEnemyStatusTexture : mPlayerStatusTexture;
  if ( lives == st.lives && bombs == st.bombs )
    return t;

  std::string text = "LIVES: " + std::to_string(lives) +
                    " BOMBS: " + std::to_string(bombs);
  SDL_DestroyTexture(t);
  st.lives = lives;
  st.bombs = bombs;
  t = UI::textToTexture(text.c_str(), {0,0,});
  if ( enemy ) mEnemyStatusTexture = t;
  else mPlayerStatusTexture = t;
  return t;
}
