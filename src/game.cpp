#include "game.h"

#include <ncurses.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <cassert>

Game::Game()
    :expired(false)
{
    Pos playerPos;
    map.load("levels/1", playerPos);
    player = new Player(this, playerPos, 3, 3);
    srand(time(0));
    loop();
}

Game::~Game()
{
    delete player;
}

void Game::loop()
{
    while (1)
    {
        if ( expired )
            break;
        handleBombs();
        handleFlames();
        int c = getch();
        if ( c != ERR )
            keyEvent(c);
        map.draw();
        drawStatus();
        refresh();
    }
}

void Game::keyEvent(int key)
{
    static bool moveButtonPressed = false;
    bool moveButtonHold = false;

    // not holding on first iteration
    static auto timestamp = getTimestamp() - std::chrono::milliseconds(1000);
    auto currentTimestamp = getTimestamp();

    moveButtonHold = (currentTimestamp - timestamp).count() < 100;
    timestamp = currentTimestamp;

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
            TimedBomb bomb;
            if ( !player->plantBomb(bomb) )
                return;
            map.at(p) = Block::typeToSymbol(Block::TIMED_BOMB);
            timedBombs.push_back(bomb);
        }
    }
    else if ( key == ' ' )
    {
        if ( player->hasRemoteBombBonus() )
            player->detonateRemoteBombs();
    }
}

void Game::drawStatus()
{
    int width, height;
    getmaxyx(stdscr, height, width);

    std::string status =
            "LIVES: " +
            std::to_string(player->getLives()) +
            " BOMBS: " +
            std::to_string(player->getBombsAvail());

    mvprintw(height-1, 0, status.c_str());
}

void Game::movePlayer(Player &p, const Pos & offset)
{
    Pos curPos = p.getPos();
    Pos newPos(curPos.x+offset.x, curPos.y+offset.y);
    if ( Block::isSolid(map.at(newPos)) )
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
    map.at(newPos) = Block::typeToSymbol(Block::PLAYER);
    p.setPos(newPos);
}

void Game::handleBombs()
{
    for ( auto it = timedBombs.begin();
          it != timedBombs.end(); )
    {
        it->newFrame();
        if ( it->shouldExplode() )
        {
            player->addBomb();
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
        it->newFrame();
        if ( it->timedOut() )
        {
            Pos pos = it->getPos();
            if ( pos == player->getPos() )
                map.at(pos) = Block::typeToSymbol(Block::PLAYER);
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
    flames.push_back(Flame(bombPos));
    map.at(bombPos) = Block::typeToSymbol(Block::FLAME);
    genFlames(bombPos + Pos(1, 0), bombPos + Pos(radius, 0));
    genFlames(bombPos + Pos(-1, 0), bombPos + Pos(-radius, 0));
    genFlames(bombPos + Pos(0, 1), bombPos + Pos(0, radius));
    genFlames(bombPos + Pos(0, -1), bombPos + Pos(0, -radius));
}

void Game::genFlames(Pos from, const Pos & to)
{
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
        char & symbol = map.at(from);
        if ( symbol == Block::typeToSymbol(Block::PLAYER) )
        {
            flames.push_back(Flame(from));
            symbol = Block::typeToSymbol(Block::FLAME);
            player->die();
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
            {
                flames.push_back(Flame(from));
                symbol = Block::typeToSymbol(Block::FLAME);
            }
            break;
        }
        else if ( symbol == Block::typeToSymbol(Block::TIMED_BOMB)
                  || symbol == Block::typeToSymbol(Block::REMOTE_BOMB) )
        {
            for ( auto it = timedBombs.begin();
                  it != timedBombs.end();
                  it++ )
                if ( it->getPos() == from )
                {
                    bombExplosion(*it);
                    player->addBomb();
                    timedBombs.erase(it);
                    return;
                }
            if ( player->detonateRemoteBomb(from) )
                return;
            else
                assert ( false );
        }
        else if ( Block::isSolid(symbol) )
            break;
        else // empty or flame
        {
            flames.push_back(Flame(from));
            symbol = Block::typeToSymbol(Block::FLAME);
        }
    }
}

std::chrono::milliseconds Game::getTimestamp() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now().time_since_epoch());
}
