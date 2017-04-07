#include "game.h"

#include <ncurses.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <functional>

#include "ui.h"
#include "ai_player.h"
#include "online_player.h"

#include <unistd.h>

Game::Game(const std::string & levelPath,
           bool enableTraps,
           bool enableGhosts,
           int lives,
           const char * address,
           const char * port)
    : mIsOnlineGame(port),
      mAddress(address ? address : ""),
      mPort(port ? port : ""),
      mGameGUI(nullptr),
      mEnableTraps(enableTraps),
      mEnableGhosts(enableGhosts),
      mLives(lives),
      mPlayer(0),
      mEnemy(0),
      mExpired(false)
#ifdef DEBUG
      ,mShouldDrawPath(false)
#endif
{
  if ( UI::mGUIMode ) mGameGUI = new GameGUI();

  if ( !load(levelPath, enableTraps, enableGhosts, lives) )
    return;

  if ( mIsOnlineGame && !initOnlineGame() )
    return;

  loop();
}

Game::~Game()
{
    delete mPlayer;
    delete mEnemy;
}

void Game::plantBombAction(Player &player)
{
    Pos p = player.getPos();
    if ( mMap.at(p) != player.getSymbol()
         && Block::isSolid(mMap.at(p)) )
        return;
    if ( player.hasRemoteBombBonus() )
    {
        player.plantRemoteBomb();
    }
    else
    {
        plantTimedBomb(player);
    }
}

bool Game::canPlantBomb(const Player &player) const
{
    if ( player.getBombsAvail() == 0 )
        return false;
    Pos p = player.getPos();
    Block::Type type = Block::symbolToType(mMap.get(p));
    if ( type != Block::PLAYER
         && type != Block::ENEMY
         && Block::isSolid(mMap.get(p)) )
        return false;
    auto trap = mTraps.find(p);
    if ( trap != mTraps.end() && trap->second.isOpen() )
        return false;
    if ( mFlames.count(p) ) // disallow bomb placement beneath flames
        return false;
    return true;
}

void Game::plantTimedBomb(Player &player)
{
    if ( !canPlantBomb(player) )
        return;
    Pos p = player.getPos();
    TimedBomb bomb(&player);
    if ( !player.plantBomb(bomb) )
        return;
    mMap.at(p) = Block::typeToSymbol(Block::TIMED_BOMB);
    mTimedBombs.push_back(bomb);
}

bool Game::initOnlineGame()
{
    mServer.setup(*mPlayer, *mEnemy, mMap);
    mServer.listen(mAddress.c_str(), mPort.c_str());

    std::function<bool(void)> f = std::bind(&Server::listeningFinished, &mServer);
    UI::Notification("Listening...",
                     "Cancel",
                     &f);

    if ( !mServer.listeningFinished() ) // cancel pressed
        return false;

    if ( !mServer.isConnected() )
    {
        UI::Notification("Error: hosting a server failed");
        return false;
    }
    return true;
}

void Game::loop()
{
  clear();
  bool firstRound = true;
  while (1)
  {
    if ( mExpired )
      break;

    if ( mIsOnlineGame && !mServer.isConnected() )
    {
      networkErrorAction();
      break;
    }

    handleBombs();
    handleFlames();

    if ( UI::mGUIMode ) keyEventGUI();
    else keyEventTUI();

    mEnemy->makeDecision();
    handleGhosts();
    handleTraps();
    assert ( mPlayer->getPos() != mEnemy->getPos() );

    drawEvent();

    if ( !firstRound && !UI::mGUIMode )
      refresh();
    else if ( mIsOnlineGame )
    {
      mServer.initOnlineGame();
      firstRound = false;
    }

    if ( mIsOnlineGame )
      mServer.update();

    if ( mPlayer->isDead() )
      loseAction();
    else if ( mEnemy->isDead() )
      winAction();
  }
}

void Game::keyEvent(int key)
{
    static bool moveButtonPressed = false;
    bool moveButtonHold = false;

    // not holding on first iteration
    static Countdown buttonCountdown(0);
    moveButtonHold = !buttonCountdown.expired(100);

    if ( key == 'q' )
    {
        mExpired = true;
    }
    else if ( key == UI::KDOWN || key == 's' )
    {
        if ( !moveButtonHold || moveButtonPressed || mPlayer->hasSpeedBonus() )
            movePlayer(*mPlayer, Pos(0,1));
        moveButtonPressed = !moveButtonPressed;
    }
    else if ( key == UI::KUP || key == 'w' )
    {
        if ( !moveButtonHold || moveButtonPressed || mPlayer->hasSpeedBonus() )
            movePlayer(*mPlayer, Pos(0,-1));
        moveButtonPressed = !moveButtonPressed;
    }
    else if ( key == UI::KLEFT || key == 'a' )
    {
        if ( !moveButtonHold || moveButtonPressed || mPlayer->hasSpeedBonus() )
            movePlayer(*mPlayer, Pos(-1,0));
        moveButtonPressed = !moveButtonPressed;
    }
    else if ( key == UI::KRIGHT || key == 'd' )
    {
        if ( !moveButtonHold || moveButtonPressed || mPlayer->hasSpeedBonus() )
            movePlayer(*mPlayer, Pos(1,0));
        moveButtonPressed = !moveButtonPressed;
    }
    else if ( key == 'b' )
    {
        plantBombAction(*mPlayer);
    }
    else if ( key == ' ' )
    {
        mPlayer->detonateRemoteBombs();
    }

#ifdef DEBUG
    else if ( key == 'p' )
    {
        mShouldDrawPath = !mShouldDrawPath;
    }
    else if ( key == 'u' )
    {
        for ( int i = 0 ; i < 40 ; i++ )
            mPlayer->die();
    }
    else if ( key == 'i' )
    {
        for ( int i = 0 ; i < 40 ; i++ )
            mEnemy->die();
    }
#endif
}

void Game::keyEventTUI()
{
  int c = getch();
  if ( c == ERR ) return;
  keyEvent(UI::convertKeyTUI(c));
}

void Game::keyEventGUI()
{
  SDL_Event e;
  if ( !SDL_PollEvent(&e) ) return;
  if ( e.type == SDL_QUIT ) keyEvent('q');
  if ( e.type == SDL_KEYDOWN )
    keyEvent(UI::convertKeyGUI(e.key.keysym.sym));
}

void Game::drawGhostsTUI() const
{
    for ( const auto & ghost : mGhosts )
        mvaddch(ghost.first.y, ghost.first.x,
                 Block::typeToSymbol(Block::GHOST));
}

void Game::drawStatusTUI() const
{
    int width, height;
    getmaxyx(stdscr, height, width);

    std::string status =
            "LIVES: " +
            std::to_string(mPlayer->getLives()) +
            " BOMBS: " +
            std::to_string(mPlayer->getBombsAvail());

    std::string enemyStatus =
            "LIVES: " +
            std::to_string(mEnemy->getLives()) +
            " BOMBS: " +
            std::to_string(mEnemy->getBombsAvail());
    mvprintw(height-1, 0, status.c_str());
    mvprintw(height-1, width-enemyStatus.size()-1, enemyStatus.c_str());
}

void Game::drawFlamesTUI() const
{
    for ( const auto & flame : mFlames )
        mvaddch(flame.first.y, flame.first.x,
                Block::typeToSymbol(Block::FLAME));
}

void Game::drawTraps() const
{
  for ( const auto & trap : mTraps )
    if ( mPlayer->getPos() != trap.first
      && mEnemy->getPos() != trap.first
      && mMap.get(Pos(trap.first)) != Block::typeToSymbol(Block::TIMED_BOMB)
      && mMap.get(Pos(trap.first)) != Block::typeToSymbol(Block::REMOTE_BOMB))
    {
      Block::Type t = trap.second.isOpen() ? Block::TRAP_OPENED
                                           : Block::TRAP_CLOSED;
      if ( UI::mGUIMode )
        mGameGUI->drawBlock(t, trap.first.x, trap.first.y);
      else
        mvaddch(trap.first.y, trap.first.x, Block::typeToSymbol(t));
    }
}

void Game::drawGhostsGUI() const
{
  for ( const auto & ghost : mGhosts )
    mGameGUI->drawBlock(Block::GHOST, ghost.first.x, ghost.first.y);
}

void Game::drawStatusGUI() const
{
  std::string status =
    "LIVES: " +
    std::to_string(mPlayer->getLives()) +
    " BOMBS: " +
    std::to_string(mPlayer->getBombsAvail());

  std::string enemyStatus =
    "LIVES: " +
    std::to_string(mEnemy->getLives()) +
    " BOMBS: " +
    std::to_string(mEnemy->getBombsAvail());

  int bsize = UI::mGUI.BLOCK_SIZE;
  SDL_Texture * texture1 = UI::textToTexture(status.c_str(), {0,0,0});
  SDL_Texture * texture2 = UI::textToTexture(enemyStatus.c_str(), {0,0,0});

  SDL_Rect rect = {0, bsize*(mMap.height()-1), 0, 0};
  SDL_QueryTexture(texture1,0,0,&rect.w,&rect.h);
  SDL_RenderCopyEx(UI::mGUI.renderer, texture1, 0, &rect, 0.0,0,SDL_FLIP_NONE);

  rect = {mMap.width()*bsize, bsize*(mMap.height()-1), 0, 0};
  SDL_QueryTexture(texture2,0,0,&rect.w,&rect.h);
  rect.x -= rect.w;
  SDL_RenderCopyEx(UI::mGUI.renderer, texture2, 0, &rect, 0.0,0,SDL_FLIP_NONE);
}

void Game::drawFlamesGUI() const
{
  for ( const auto & flame : mFlames )
    mGameGUI->drawBlock(Block::FLAME, flame.first.x, flame.first.y);
}

void Game::drawPlayersGUI() const
{
  mGameGUI->drawPlayer(*mPlayer);
  mGameGUI->drawEnemy(*mEnemy);
}

void Game::drawEvent() const
{
  if ( UI::mGUIMode ) {
    SDL_SetRenderDrawColor(UI::mGUI.renderer, 76, 65, 65, 0xFF);
    SDL_RenderClear(UI::mGUI.renderer);
    mMap.drawGUI();
    drawPlayersGUI();
    drawTraps();
    drawGhostsGUI();
    drawFlamesGUI();
  } else {
    mMap.drawTUI();
    drawTraps();
    drawGhostsTUI();
    drawFlamesTUI();
  }

#ifdef DEBUG
  if ( mShouldDrawPath )
    drawPath();
#endif

  if ( UI::mGUIMode ) {
    drawStatusGUI();
    SDL_RenderPresent(UI::mGUI.renderer);
  }
  else drawStatusTUI();
}

void Game::movePlayer(Player &player, const Pos & offset)
{
    Pos curPos = player.getPos();
    Pos newPos = curPos + offset;
    if ( !canMovePlayer(newPos) )
        return;
    Block::Type newPosType = Block::symbolToType(mMap.at(newPos));
    if ( newPosType >= Block::BONUS_BOMB &&
         newPosType <= Block::BONUS_RADIUS )
    {
        if ( newPosType == Block::BONUS_BOMB )
        {
            player.addBomb();
        }
        else if ( newPosType == Block::BONUS_SPEED )
        {
            player.setSpeedBonus(true);
        }
        else if ( newPosType == Block::BONUS_REMOTE )
        {
            player.setRemoteBombBonus(true);
        }
        else // BONUS_RADIUS
        {
            player.upgradeBombRadius();
        }
        mBonuses.erase(posKey(newPos));
    }

    if ( mMap.at(curPos) != Block::typeToSymbol(Block::TIMED_BOMB)
         && mMap.at(curPos) != Block::typeToSymbol(Block::REMOTE_BOMB) )
        mMap.at(curPos) = Block::typeToSymbol(Block::EMPTY);

    if ( mFlames.find(newPos) != mFlames.end() )
        player.die();

    if ( mGhosts.find(newPos) != mGhosts.end() && &player == mPlayer )
        player.die();

    auto trap = mTraps.find(newPos);
    if ( trap != mTraps.end() && trap->second.isOpen() )
        player.die();

    mMap.at(newPos) = player.getSymbol();
    player.move(offset);
}

bool Game::canMoveGhost(const Pos & where) const
{
    if ( !mMap.withinBounds(where) )
        return false;
    return mMap.get(where) != Block::typeToSymbol(Block::WALL)
            && mMap.get(where) != Block::typeToSymbol(Block::NICE_WALL)
            && mGhosts.find(where) == mGhosts.end();
}

bool Game::moveGhost(Ghost &ghost, const Pos & offset)
{
    Pos newPos = ghost.getPos() + offset;
    ghost.setPos(newPos);

    if ( mFlames.find(newPos) != mFlames.end() )
        return false;

    if ( mPlayer->getPos() == newPos )
        mPlayer->die();

    if ( mIsOnlineGame && mEnemy->getPos() == newPos )
        mEnemy->die();

    return true;
}

Map &Game::getMap()
{
    return mMap;
}

const Bomb * Game::getBomb(const Pos &pos) const
{
    for ( auto it = mTimedBombs.begin();
          it != mTimedBombs.end();
          it++ )
    {
        if ( it->getPos() == pos )
            return &*it;
    }
    const Bomb * b = mPlayer->getRemoteBomb(pos);
    if ( b )
        return b;
    return mEnemy->getRemoteBomb(pos);
}

const TimedBomb &Game::getTimedBomb(const Pos &pos) const
{
    for ( auto it = mTimedBombs.begin();
          it != mTimedBombs.end(); )
    {
        if ( it->getPos() == pos )
            return *it;
    }
    assert ( false );
}

bool Game::isFlameAt(const Pos &pos) const
{
    return mFlames.find(pos) != mFlames.end();
}

const Trap * Game::trapAt(const Pos &p) const
{
    auto found = mTraps.find(p);
    if ( found != mTraps.end() )
        return &found->second;
    else
        return 0;
}

void Game::handleBombs()
{
    for ( auto it = mTimedBombs.begin();
          it != mTimedBombs.end(); )
    {
        if ( it->expired() )
        {
            it->getOwner()->addBomb();
            bombExplosion(*it);
            it = mTimedBombs.erase(it);
            continue;
        }
        it++;
    }
}

void Game::handleFlames()
{
    for ( auto it = mFlames.begin();
          it != mFlames.end(); )
    {
        if ( it->second.expired() )
        {
            Pos pos = it->second.getPos();
            if ( pos == mPlayer->getPos() )
                mMap.at(pos) = Block::typeToSymbol(Block::PLAYER);
            else if ( pos == mEnemy->getPos() )
                mMap.at(pos) = Block::typeToSymbol(Block::ENEMY);
            else
                mMap.at(pos) = Block::typeToSymbol(Block::EMPTY);
            it = mFlames.erase(it);
            continue;
        }
        it++;
    }
}

void Game::bombExplosion(const Bomb & bomb)
{
    Pos bombPos = bomb.getPos();
    int radius = bomb.getRadius();

    if ( bombPos == mPlayer->getPos() )
        mMap.at(bombPos) = Block::typeToSymbol(Block::PLAYER);
    else if ( bombPos == mEnemy->getPos() )
        mMap.at(bombPos) = Block::typeToSymbol(Block::ENEMY);
    else
        mMap.at(bombPos) = Block::typeToSymbol(Block::EMPTY);

    if ( mPlayer->getPos() == bombPos )
        mPlayer->die();
    if ( mEnemy->getPos() == bombPos )
        mEnemy->die();
    mFlames.insert(std::make_pair(bombPos, Flame(bombPos)));
    genFlames(bombPos + Pos(1, 0), bombPos + Pos(radius, 0));
    genFlames(bombPos + Pos(-1, 0), bombPos + Pos(-radius, 0));
    genFlames(bombPos + Pos(0, 1), bombPos + Pos(0, radius));
    genFlames(bombPos + Pos(0, -1), bombPos + Pos(0, -radius));
}

bool Game::canMovePlayer(const Pos & where) const
{
    if ( !mMap.withinBounds(where) )
        return false;
    char symbol = mMap.get(where);
    if ( Block::isSolid(symbol) )
        return false;
    return true;
}

void Game::genFlames(Pos from, const Pos & to)
{
    //todo refactor
    //todo asserts

    Pos diff;
    if ( from.x < to.x )
        diff = Pos(1, 0);
    else if ( from.x > to.x )
        diff = Pos(-1, 0);
    else if ( from.y < to.y )
        diff = Pos(0, 1);
    else // up direction and possibly only one flame
        diff = Pos(0, -1);

    for ( ; from != to ; from += diff )
    {
        if ( !mMap.withinBounds(from) )
            return;
        auto it = mGhosts.find(from);
        if ( it != mGhosts.end() )
            mGhosts.erase(it);

        using std::make_pair;
        char & symbol = mMap.at(from);
        if ( symbol == Block::typeToSymbol(Block::PLAYER) )
        {
            mFlames.insert(make_pair(from, Flame(from)));
            mPlayer->die();
        }
        else if ( symbol == Block::typeToSymbol(Block::ENEMY) )
        {
            mFlames.insert(make_pair(from, Flame(from)));
            mEnemy->die();
        }
        else if ( symbol == Block::typeToSymbol(Block::DESTRUCTABLE) )
        {
            if ( rand() % 10 == 0 )
            {
                int key = posKey(from);
                int type = rand() % 4 + Block::BONUS_BOMB;
                mBonuses.insert(std::make_pair(key, Bonus((Block::Type)type)));
                symbol = Block::typeToSymbol(Block::Type(type));
            }
            else
                mFlames.insert(make_pair(from, Flame(from)));
            break;
        }
        else if ( symbol == Block::typeToSymbol(Block::TIMED_BOMB)
                  || symbol == Block::typeToSymbol(Block::REMOTE_BOMB) )
        {
            /* todo: simplify using getBomb() */
            for ( auto it = mTimedBombs.begin();
                  it != mTimedBombs.end();
                  it++ )
                if ( it->getPos() == from )
                {
                    Bomb tmp = *it;
                    it->getOwner()->addBomb();
                    mTimedBombs.erase(it);
                    bombExplosion(tmp);
                    return;
                }
            if ( mPlayer->detonateRemoteBomb(from)
                 || mEnemy->detonateRemoteBomb(from) )
                return;
            else
                assert ( false );
        }
        else if ( Block::isSolid(symbol) )
            break;
        else // empty or flame or trap
        {
            mFlames.insert(make_pair(from, Flame(from)));
        }
    }
}

void Game::getCandidates(std::vector<Pos> & out) const
{
    for ( int i = 0 ; i < mMap.height() ; i++ )
        for ( int j = 0 ; j < mMap.width() ; j++ )
            if ( mMap.get(Pos(j, i))
                 == Block::typeToSymbol(Block::DESTRUCTABLE) )
                out.push_back(Pos(j, i));
}

void Game::genGhosts(std::vector<Pos> candidates)
{
    int nGhosts = mMap.width() * mMap.height() / 100;
    for ( int i = 0 ; i < nGhosts ; i++ )
    {
        if ( candidates.empty() )
            return;
        int id = rand() % candidates.size();
        Pos c = candidates[id];
        if ( 7 < Pos::manhattanDistance(c, mPlayer->getPos())
             && 7 < Pos::manhattanDistance(c, mEnemy->getPos()) )
            mGhosts.insert(std::make_pair(c, Ghost(this, c)));
        candidates.erase(candidates.begin() + id);
    }
}

void Game::handleGhosts()
{
    static Countdown moveCountdown(500);
    if ( !moveCountdown.expired() )
        return;
    std::map<Pos, Ghost> tmp = mGhosts;
    for ( auto ghost = tmp.begin();
          ghost != tmp.end();
          ghost++ )
    {
        if ( ghost->second.makeDecision() )
        {
            Pos newPos = ghost->second.getPos();
            auto it = mGhosts.find(ghost->first);

            // the bomb may have been already removed by makeDecision()
            // which might caused player's death resulting in detonating
            // his bombs which could have caused ghosts being removed
            if ( it == mGhosts.end() )
                continue;

            mGhosts.erase(it);
            mGhosts.insert(std::make_pair(newPos, ghost->second));
        }
        else
            mGhosts.erase(ghost->first);
    }
}

void Game::genTraps(std::vector<Pos> candidates)
{
    int nTraps = mMap.width() * mMap.height() / 100;
    for ( int i = 0 ; i < nTraps ; i++ )
    {
        if ( candidates.empty() )
            return;
        int id = rand() % candidates.size();
        Pos c = candidates[id];
        if ( 7 < Pos::manhattanDistance(c, mPlayer->getPos())
             && 7 < Pos::manhattanDistance(c, mEnemy->getPos()) )
        {
            mMap.at(c) = Block::typeToSymbol(Block::EMPTY);
            mTraps.insert(std::make_pair(c, Trap(mMap, c)));
        }
        candidates.erase(candidates.begin() + id);
    }

}

void Game::handleTraps()
{
    for ( auto & trap : mTraps )
        trap.second.update();
}

int Game::posKey(const Pos &pos)
{
    return (pos.x << 2) + pos.y;
}

std::chrono::milliseconds Game::getTimestamp()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now().time_since_epoch());
}

bool Game::load(const std::string & levelPath,
                bool enableTraps,
                bool enableGhosts,
                int lives)
{
    mLevelPath = levelPath;
    Pos playerPos;
    Pos enemyPos;
    std::set<Pos> trapsPos;

    delete mPlayer;
    mPlayer = 0;
    delete mEnemy;
    mEnemy = 0;
    mTraps.clear();
    mTimedBombs.clear();
    mFlames.clear();
    mBonuses.clear();
    mGhosts.clear();
    mTraps.clear();

    try
    {
        mMap.load(levelPath.c_str(), playerPos, enemyPos, trapsPos, mGameGUI);
    }
    catch ( ... )
    {
        return false;
    }

    mPlayer = new Player(this, playerPos, lives, 3);

    if ( mIsOnlineGame )
        mEnemy = new OnlinePlayer(this, enemyPos, lives, 3, &mServer);
    else
        mEnemy = new AIPlayer(this, mPlayer, enemyPos, lives, 3);

    std::vector<Pos> candidates;
    getCandidates(candidates);

    if ( enableGhosts )
        genGhosts(candidates);

    if ( !trapsPos.empty() )
        for ( const auto & trap : trapsPos )
            mTraps.insert(std::make_pair(trap, Trap(mMap, trap)));

    if ( enableTraps )
        Game::genTraps(candidates);

    clear();

    return true;
}

void Game::winAction()
{
    UI::Notification("You won!");

    if ( mIsOnlineGame )
    {
        mExpired = true;
        return;
    }

    std::string lvlPath = mLevelPath;
    int lvlNr = lvlPath[lvlPath.size()-1]-48;
    lvlPath[lvlPath.size()-1] = (lvlNr+1+48);

    if ( !load(lvlPath, mEnableTraps, mEnableGhosts, mLives) )
        mExpired = true;
}

void Game::loseAction()
{
    UI::Notification("You lost!");

    if ( mIsOnlineGame )
        mExpired = true;
    else
        load(mLevelPath, mEnableTraps, mEnableGhosts, mLives);
}

void Game::networkErrorAction()
{
    UI::Notification("Network error");
}

#ifdef DEBUG
void Game::drawPath() const
{
    if ( mIsOnlineGame )
        return;
    for ( const auto & block : dynamic_cast<AIPlayer*>(mEnemy)->mPath )
        mvaddch(block.y, block.x, '~');
}
#endif
