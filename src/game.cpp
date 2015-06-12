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
      mEnableTraps(enableTraps),
      mEnableGhosts(enableGhosts),
      mLives(lives),
      player(0),
      enemy(0),
      expired(false),
      shouldDrawPath(false)
{
    if ( !load(levelPath, enableTraps, enableGhosts, lives) )
        return;

    if ( mIsOnlineGame )
    {
        mServer.setup(*player, *enemy, map);
        mServer.listen(address, port);

        std::function<bool(void)> f = std::bind(&Server::listeningFinished, &mServer);
        UI::Notification("Listening...",
                         "Cancel",
                         &f);

        if ( !mServer.listeningFinished() ) // cancel pressed
            return;

        if ( !mServer.isConnected() )
        {
            UI::Notification("Error: hosting a server failed");
            return;
        }
    }

    loop();
}

Game::~Game()
{
    delete player;
    delete enemy;
}

void Game::plantBombAction(Player &player)
{
    Pos p = player.getPos();
    if ( map.at(p) != player.getSymbol()
         && Block::isSolid(map.at(p)) )
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
    Block::Type type = Block::symbolToType(map.get(p));
    if ( type != Block::PLAYER
         && type != Block::ENEMY
         && Block::isSolid(map.get(p)) )
        return false;
    auto trap = mTraps.find(p);
    if ( trap != mTraps.end() && trap->second.isOpen() )
        return false;
    if ( flames.count(p) ) // disallow bomb placement beneath flames
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
    map.at(p) = Block::typeToSymbol(Block::TIMED_BOMB);
    timedBombs.push_back(bomb);
}

void Game::loop()
{
    clear();
    bool firstRound = true;
    while (1)
    {
        if ( expired )
            break;

        if ( mIsOnlineGame && !mServer.isConnected() )
        {
            networkErrorAction();
            break;
        }

        handleBombs();
        handleFlames();
        int c = getch();
        if ( c != ERR )
            keyEvent(c);

        enemy->makeDecision();
        handleGhosts();
        handleTraps();
        assert ( player->getPos() != enemy->getPos() );

        map.draw();
        drawTraps();
        drawGhosts();
        drawFlames();
        if ( shouldDrawPath )
            drawPath();
        drawStatus();

        if ( !firstRound )
            refresh();
        else if ( mIsOnlineGame )
        {
            mServer.initOnlineGame();
            firstRound = false;
        }

        if ( mIsOnlineGame )
            mServer.update();

        if ( player->isDead() )
            loseAction();
        else if ( enemy->isDead() )
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
        expired = true;
    }
    else if ( key == KEY_DOWN || key == 's' )
    {
        if ( !moveButtonHold || moveButtonPressed || player->hasSpeedBonus() )
            movePlayer(*player, Pos(0,1));
        moveButtonPressed = !moveButtonPressed;
    }
    else if ( key == KEY_UP || key == 'w' )
    {
        if ( !moveButtonHold || moveButtonPressed || player->hasSpeedBonus() )
            movePlayer(*player, Pos(0,-1));
        moveButtonPressed = !moveButtonPressed;
    }
    else if ( key == KEY_LEFT || key == 'a' )
    {
        if ( !moveButtonHold || moveButtonPressed || player->hasSpeedBonus() )
            movePlayer(*player, Pos(-1,0));
        moveButtonPressed = !moveButtonPressed;
    }
    else if ( key == KEY_RIGHT || key == 'd' )
    {
        if ( !moveButtonHold || moveButtonPressed || player->hasSpeedBonus() )
            movePlayer(*player, Pos(1,0));
        moveButtonPressed = !moveButtonPressed;
    }
    else if ( key == 'b' )
    {
        plantBombAction(*player);
    }
    else if ( key == ' ' )
    {
        player->detonateRemoteBombs();
    }
    else if ( key == 'p' ) // debug
    {
        shouldDrawPath = !shouldDrawPath;
    }
    else if ( key == 'u' )
    {
        for ( int i = 0 ; i < 40 ; i++ )
            player->die();
    }
    else if ( key == 'i' )
    {
        for ( int i = 0 ; i < 40 ; i++ )
            enemy->die();
    }
}

void Game::drawGhosts() const
{
    for ( const auto & ghost : mGhosts )
        mvaddch(ghost.first.y, ghost.first.x,
                 Block::typeToSymbol(Block::GHOST));
}

void Game::drawStatus() const
{
    int width, height;
    getmaxyx(stdscr, height, width);

    std::string status =
            "LIVES: " +
            std::to_string(player->getLives()) +
            " BOMBS: " +
            std::to_string(player->getBombsAvail());

    std::string enemyStatus =
            "LIVES: " +
            std::to_string(enemy->getLives()) +
            " BOMBS: " +
            std::to_string(enemy->getBombsAvail());
    mvprintw(height-1, 0, status.c_str());
    mvprintw(height-1, width-enemyStatus.size()-1, enemyStatus.c_str());
}

void Game::drawFlames() const
{
    for ( const auto & flame : flames )
        mvaddch(flame.first.y, flame.first.x,
                Block::typeToSymbol(Block::FLAME));
}

void Game::drawTraps() const
{
    for ( const auto & trap : mTraps )
        if ( player->getPos() != trap.first
             && enemy->getPos() != trap.first
             && map.get(Pos(trap.first)) != Block::typeToSymbol(Block::TIMED_BOMB)
             && map.get(Pos(trap.first)) != Block::typeToSymbol(Block::REMOTE_BOMB))
            mvaddch(trap.first.y, trap.first.x,
                    trap.second.isOpen() ?
                        Block::typeToSymbol(Block::TRAP_OPENED)
                      : Block::typeToSymbol(Block::TRAP_CLOSED));
}

void Game::movePlayer(Player &p, const Pos & offset)
{
    Pos curPos = p.getPos();
    Pos newPos = curPos + offset;
    if ( !canMovePlayer(newPos) )
        return;
    Block::Type newPosType = Block::symbolToType(map.at(newPos));
    if ( newPosType >= Block::BONUS_BOMB &&
         newPosType <= Block::BONUS_RADIUS )
    {
        if ( newPosType == Block::BONUS_BOMB )
        {
            p.addBomb();
        }
        else if ( newPosType == Block::BONUS_SPEED )
        {
            p.setSpeedBonus(true);
        }
        else if ( newPosType == Block::BONUS_REMOTE )
        {
            p.setRemoteBombBonus(true);
        }
        else // BONUS_RADIUS
        {
            p.upgradeBombRadius();
        }
        bonuses.erase(Bonus::key(newPos));
    }

    if ( map.at(curPos) != Block::typeToSymbol(Block::TIMED_BOMB)
         && map.at(curPos) != Block::typeToSymbol(Block::REMOTE_BOMB) )
        map.at(curPos) = Block::typeToSymbol(Block::EMPTY);

    if ( flames.find(newPos) != flames.end() )
        p.die();

    if ( mGhosts.find(newPos) != mGhosts.end() && &p == player )
        p.die();

    auto trap = mTraps.find(newPos);
    if ( trap != mTraps.end() && trap->second.isOpen() )
        p.die();

    map.at(newPos) = p.getSymbol();
    p.setPos(newPos);
}

bool Game::canMoveGhost(const Pos & where) const
{
    if ( !withinBounds(where) )
        return false;
    return map.get(where) != Block::typeToSymbol(Block::WALL)
            && map.get(where) != Block::typeToSymbol(Block::NICE_WALL)
            && mGhosts.find(where) == mGhosts.end();
}

/* call canMoveGhost before this */
bool Game::moveGhost(Ghost &g, const Pos & offset)
{
    Pos newPos = g.getPos() + offset;
    g.setPos(newPos);

    if ( flames.find(newPos) != flames.end() )
        return false;

    if ( player->getPos() == newPos )
        player->die();

    if ( mIsOnlineGame && enemy->getPos() == newPos )
        enemy->die();

    return true;
}

Map &Game::getMap()
{
    return map;
}

const Bomb * Game::getBomb(const Pos &p) const
{
    for ( auto it = timedBombs.begin();
          it != timedBombs.end();
          it++ )
    {
        if ( it->getPos() == p )
            return &*it;
    }
    const Bomb * b = player->getRemoteBomb(p);
    if ( b )
        return b;
    return enemy->getRemoteBomb(p);
}

const TimedBomb &Game::getTimedBomb(const Pos &p) const
{
    for ( auto it = timedBombs.begin();
          it != timedBombs.end(); )
    {
        if ( it->getPos() == p )
            return *it;
    }
    assert ( false );
}

bool Game::isFlameAt(const Pos &p) const
{
    return flames.find(p) != flames.end();
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
    for ( auto it = timedBombs.begin();
          it != timedBombs.end(); )
    {
        it->newFrame();
        if ( it->shouldExplode() )
        {
            it->getOwner()->addBomb();
            bombExplosion(*it);
            it = timedBombs.erase(it);
            continue;
        }
        it++;
    }
}

void Game::handleFlames()
{
    for ( auto it = flames.begin();
          it != flames.end(); )
    {
        it->second.newFrame();
        if ( it->second.timedOut() )
        {
            Pos pos = it->second.getPos();
            if ( pos == player->getPos() )
                map.at(pos) = Block::typeToSymbol(Block::PLAYER);
            else if ( pos == enemy->getPos() )
                map.at(pos) = Block::typeToSymbol(Block::ENEMY);
            else
                map.at(pos) = Block::typeToSymbol(Block::EMPTY);
            it = flames.erase(it);
            continue;
        }
        it++;
    }
}

void Game::bombExplosion(const Bomb & b)
{
    Pos bombPos = b.getPos();
    int radius = b.getRadius();

    if ( bombPos == player->getPos() )
        map.at(bombPos) = Block::typeToSymbol(Block::PLAYER);
    else if ( bombPos == enemy->getPos() )
        map.at(bombPos) = Block::typeToSymbol(Block::ENEMY);
    else
        map.at(bombPos) = Block::typeToSymbol(Block::EMPTY);

    if ( player->getPos() == bombPos )
        player->die();
    if ( enemy->getPos() == bombPos )
        enemy->die();
    flames.insert(std::make_pair(bombPos, Flame(bombPos)));
    genFlames(bombPos + Pos(1, 0), bombPos + Pos(radius, 0));
    genFlames(bombPos + Pos(-1, 0), bombPos + Pos(-radius, 0));
    genFlames(bombPos + Pos(0, 1), bombPos + Pos(0, radius));
    genFlames(bombPos + Pos(0, -1), bombPos + Pos(0, -radius));
}

bool Game::canMovePlayer(const Pos & where) const
{
    if ( !withinBounds(where) )
        return false;
    char symbol = map.get(where);
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
        if ( !withinBounds(from) )
            return;
        auto it = mGhosts.find(from);
        if ( it != mGhosts.end() )
            mGhosts.erase(it);

        using std::make_pair;
        char & symbol = map.at(from);
        if ( symbol == Block::typeToSymbol(Block::PLAYER) )
        {
            flames.insert(make_pair(from, Flame(from)));
            player->die();
        }
        else if ( symbol == Block::typeToSymbol(Block::ENEMY) )
        {
            flames.insert(make_pair(from, Flame(from)));
            enemy->die();
        }
        else if ( symbol == Block::typeToSymbol(Block::DESTRUCTABLE) )
        {
            if ( rand() % 5 == 0 )
            {
                int key = Bonus::key(from);
                int type = rand() % 4 + Block::BONUS_BOMB;
                bonuses.insert(std::make_pair(key, Bonus((Block::Type)type)));
                symbol = Block::typeToSymbol(Block::Type(type));
            }
            else
                flames.insert(make_pair(from, Flame(from)));
            break;
        }
        else if ( symbol == Block::typeToSymbol(Block::TIMED_BOMB)
                  || symbol == Block::typeToSymbol(Block::REMOTE_BOMB) )
        {
            /* todo: simplify using getBomb() */
            for ( auto it = timedBombs.begin();
                  it != timedBombs.end();
                  it++ )
                if ( it->getPos() == from )
                {
                    Bomb tmp = *it;
                    it->getOwner()->addBomb();
                    timedBombs.erase(it);
                    bombExplosion(tmp);
                    return;
                }
            if ( player->detonateRemoteBomb(from)
                 || enemy->detonateRemoteBomb(from) )
                return;
            else
                assert ( false );
        }
        else if ( Block::isSolid(symbol) )
            break;
        else // empty or flame or trap
        {
            flames.insert(make_pair(from, Flame(from)));
        }
    }
}

void Game::getCandidates(std::vector<Pos> & out) const
{
    for ( int i = 0 ; i < map.height() ; i++ )
        for ( int j = 0 ; j < map.width() ; j++ )
            if ( map.get(Pos(j, i))
                 == Block::typeToSymbol(Block::DESTRUCTABLE) )
                out.push_back(Pos(j, i));
}

void Game::genGhosts(std::vector<Pos> candidates)
{
    int nGhosts = map.width() * map.height() / 100;
    for ( int i = 0 ; i < nGhosts ; i++ )
    {
        if ( candidates.empty() )
            return;
        int id = rand() % candidates.size();
        Pos c = candidates[id];
        if ( 7 < Pos::manhattanDistance(c, player->getPos())
             && 7 < Pos::manhattanDistance(c, enemy->getPos()) )
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
            mGhosts.erase(mGhosts.find(ghost->first));
            mGhosts.insert(std::make_pair(newPos, ghost->second));
        }
        else
            mGhosts.erase(ghost->first);
    }
}

void Game::genTraps(std::vector<Pos> candidates)
{
    int nTraps = map.width() * map.height() / 100;
    for ( int i = 0 ; i < nTraps ; i++ )
    {
        if ( candidates.empty() )
            return;
        int id = rand() % candidates.size();
        Pos c = candidates[id];
        if ( 7 < Pos::manhattanDistance(c, player->getPos())
             && 7 < Pos::manhattanDistance(c, enemy->getPos()) )
        {
            map.at(c) = Block::typeToSymbol(Block::EMPTY);
            mTraps.insert(std::make_pair(c, Trap(map, c)));
        }
        candidates.erase(candidates.begin() + id);
    }

}

void Game::handleTraps()
{
    for ( auto & trap : mTraps )
        trap.second.update();
}

bool Game::withinBounds(const Pos &pos) const
{
    return pos.x > 0 && pos.x < map.width()-1
            && pos.y > 0 && pos.y < map.height()-1;
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

    delete player;
    delete enemy;
    mTraps.clear();
    timedBombs.clear();
    flames.clear();
    bonuses.clear();
    mGhosts.clear();
    mTraps.clear();

    try
    {
        map.load(levelPath.c_str(), playerPos, enemyPos, trapsPos);
    }
    catch ( ... )
    {
        return false;
    }

    player = new Player(this, playerPos, lives, 3);

    if ( mIsOnlineGame )
        enemy = new OnlinePlayer(this, enemyPos, lives, 3, &mServer);
    else
        enemy = new AIPlayer(this, player, enemyPos, lives, 3);

    std::vector<Pos> candidates;
    getCandidates(candidates);

    if ( enableGhosts )
        genGhosts(candidates);

    if ( !trapsPos.empty() )
        for ( const auto & trap : trapsPos )
            mTraps.insert(std::make_pair(trap, Trap(map, trap)));

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
        expired = true;
        return;
    }

    std::string lvlPath = mLevelPath;
    int lvlNr = lvlPath[lvlPath.size()-1]-48;
    lvlPath[lvlPath.size()-1] = (lvlNr+1+48);
    load(lvlPath, mEnableTraps, mEnableGhosts, mLives);
}

void Game::loseAction()
{
    UI::Notification("You lost!");

    if ( mIsOnlineGame )
        expired = true;
    else
        load(mLevelPath, mEnableTraps, mEnableGhosts, mLives);
}

void Game::networkErrorAction()
{
    UI::Notification("Network error");
}

void Game::drawPath() const
{
   // for ( const auto & block : enemy->mPath )
   //     mvaddch(block.y, block.x, '~');
}
