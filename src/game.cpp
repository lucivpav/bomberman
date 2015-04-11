#include "game.h"

#include <ncurses.h>
#include <string>

Game::Game()
    :expired(false)
{
    Pos playerPos;
    map.load("levels/1", playerPos);
    player = new Player(playerPos, 3, 3);
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
    if ( key == 'q' )
    {
        expired = true;
    }
    else if ( key == KEY_DOWN || key == 's' )
    {
        movePlayer(*player, Pos(0,1));
    }
    else if ( key == KEY_UP || key == 'w' )
    {
        movePlayer(*player, Pos(0,-1));
    }
    else if ( key == KEY_LEFT || key == 'a' )
    {
        movePlayer(*player, Pos(-1,0));
    }
    else if ( key == KEY_RIGHT || key == 'd' )
    {
        movePlayer(*player, Pos(1,0));
    }
    else if ( key == 'b' )
    {
        Pos p = player->getPos();
        if ( map.at(p) == Block::typeToSymbol(Block::BOMB) ||
             !player->plantBomb() )
            return;
        map.at(p) = Block::typeToSymbol(Block::BOMB);
        bombs.push_back(Bomb(p, 3));
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

    if ( map.at(curPos) != Block::typeToSymbol(Block::BOMB) )
        map.at(curPos) = Block::typeToSymbol(Block::EMPTY);
    map.at(newPos) = Block::typeToSymbol(Block::PLAYER);
    p.setPos(newPos);
}

void Game::handleBombs()
{
    for ( std::vector<Bomb>::iterator it = bombs.begin();
          it != bombs.end(); )
    {
        it->newFrame();
        if ( it->shouldExplode() )
        {
            Pos bombPos = it->getPos();
            int strength = it->strength();
            player->addBomb();
            it = bombs.erase(it);

            flames.push_back(Flame(bombPos));
            map.at(bombPos) = Block::typeToSymbol(Block::FLAME);
            genFlames(bombPos + Pos(1, 0), bombPos + Pos(strength, 0));
            genFlames(bombPos + Pos(-1, 0), bombPos + Pos(-strength, 0));
            genFlames(bombPos + Pos(0, 1), bombPos + Pos(0, strength));
            genFlames(bombPos + Pos(0, -1), bombPos + Pos(0, -strength));
            continue;
        }
        it++;
    }
}

void Game::handleFlames()
{
    for ( std::vector<Flame>::iterator it = flames.begin();
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
void Game::genFlames(Pos from, const Pos & to)
{
    //todo asserts

    Pos diff;
    if ( from.x < to.x )
        diff = Pos(1, 0);
    else if ( from.x > to.y )
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
            flames.push_back(Flame(from));
            symbol = Block::typeToSymbol(Block::FLAME);
            break;
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
