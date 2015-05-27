#include "game.h"

#include <ncurses.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cassert>

Game::Game(bool genTraps)
    :expired(false)
{
    Pos playerPos;
    Pos enemyPos;
    map.load("levels/1", playerPos, enemyPos);
    player = new Player(this, playerPos, 3, 3);
    enemy = new AIPlayer(this, player, enemyPos, 3, 3);
    srand(time(0));
    genGhosts();
    if ( genTraps )
        Game::genTraps();
    loop();
}

Game::~Game()
{
    delete player;
    delete enemy;
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
    if ( mTraps.find(p) != mTraps.end() )
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
    while (1)
    {
        if ( expired )
            break;
        handleBombs();
        handleFlames();
        int c = getch();
        if ( c != ERR )
            keyEvent(c);

        enemy->makeDecision();
        handleGhosts();
        handleTraps();

        map.draw();
        drawTraps();
        drawGhosts();
        drawFlames();
        drawStatus();
        refresh();
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
        Pos p = player->getPos();
        if ( Block::symbolToType(map.at(p)) != Block::PLAYER
             && Block::isSolid(map.at(p)) )
            return;
        if ( player->hasRemoteBombBonus() )
        {
            if ( !player->plantRemoteBomb() )
                return;
            map.at(p) = Block::typeToSymbol(Block::REMOTE_BOMB);
        }
        else
        {
            plantTimedBomb(*player);
        }
    }
    else if ( key == ' ' )
    {
        if ( player->hasRemoteBombBonus() )
            player->detonateRemoteBombs();
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
             && enemy->getPos() != trap.first )
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
    return map.get(where) != Block::typeToSymbol(Block::WALL)
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
    return true;
}

const Map &Game::getMap() const
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

bool Game::isTrapAt(const Pos &p) const
{
    return mTraps.find(p) != mTraps.end();
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
    if ( player->getPos() == bombPos )
        player->die();
    if ( enemy->getPos() == bombPos )
        enemy->die();
    map.at(bombPos) = Block::typeToSymbol(Block::EMPTY);
    flames.insert(std::make_pair(bombPos, Flame(bombPos)));
    genFlames(bombPos + Pos(1, 0), bombPos + Pos(radius, 0));
    genFlames(bombPos + Pos(-1, 0), bombPos + Pos(-radius, 0));
    genFlames(bombPos + Pos(0, 1), bombPos + Pos(0, radius));
    genFlames(bombPos + Pos(0, -1), bombPos + Pos(0, -radius));
}

bool Game::canMovePlayer(const Pos & where) const
{
    char symbol = map.get(where);
    if ( Block::isSolid(symbol)
         && symbol != Block::typeToSymbol(Block::PLAYER)
         && symbol != Block::typeToSymbol(Block::ENEMY) )
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

void Game::genGhosts()
{
    int nGhosts = map.width() * map.height() / 100;
    for ( int i = 0 ; i < nGhosts ; i++ )
        genGhost();
}

void Game::genGhost()
{
    Pos ghostPos = Pos(rand() % map.width(), rand() % map.height());
    char ground = map.get(ghostPos);
    if ( ground == Block::typeToSymbol(Block::WALL)
         || ground == Block::typeToSymbol(Block::ENEMY)
         || mGhosts.find(ghostPos) != mGhosts.end()
         || 10 > Pos::manhattanDistance(ghostPos, player->getPos()) )
    {
        genGhost();
        return;
    }
    mGhosts.insert(std::make_pair(ghostPos, Ghost(this, ghostPos)));
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

void Game::genTraps()
{
    int nTraps = map.width() * map.height() / 100;
    for ( int i = 0 ; i < nTraps ; i++ )
        genTrap();
}

void Game::genTrap()
{
    Pos trapPos = Pos(rand() % map.width(), rand() % map.height());
    char ground = map.get(trapPos);
    if ( ground == Block::typeToSymbol(Block::WALL)
         || ground == Block::typeToSymbol(Block::ENEMY)
         || ground == Block::typeToSymbol(Block::TRAP_CLOSED)
         || 10 > Pos::manhattanDistance(trapPos, player->getPos()) )
    {
        genTrap();
        return;
    }
    map.at(trapPos) = Block::typeToSymbol(Block::EMPTY);
    mTraps.insert(std::make_pair(trapPos, Trap(map, trapPos)));
}

void Game::handleTraps()
{
    for ( auto & trap : mTraps )
        trap.second.update();
}
