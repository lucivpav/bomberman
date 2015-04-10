#include "game.h"

#include <ncurses.h>

Game::Game()
{
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    cbreak();
    curs_set(FALSE);
    timeout(33);

    Pos playerPos;
    map.load("levels/1", playerPos);
    player.setPos(playerPos);
    loop();
}

Game::~Game()
{
    endwin();
}

void Game::loop()
{
    while (1)
    {
        for ( std::vector<Bomb>::iterator it = bombs.begin();
              it != bombs.end(); )
        {
            it->newFrame();
            if ( it->shouldExplode() )
            {
                map.at(it->getPos()) = Block::typeToSymbol(Block::EMPTY);
                it = bombs.erase(it);
                continue;
            }
            it++;
        }
        int c = getch();
        if ( c == 'q' )
            break;
        keyEvent(c);
        map.draw();
        refresh();
    }
}

void Game::keyEvent(int key)
{
    if ( key == KEY_DOWN || key == 's' )
    {
        movePlayer(player, Pos(0,1));
    }
    else if ( key == KEY_UP || key == 'w' )
    {
        movePlayer(player, Pos(0,-1));
    }
    else if ( key == KEY_LEFT || key == 'a' )
    {
        movePlayer(player, Pos(-1,0));
    }
    else if ( key == KEY_RIGHT || key == 'd' )
    {
        movePlayer(player, Pos(1,0));
    }
    else if ( key == 'b' )
    {
        if ( !bombs.empty() )
            return;
        Pos p = player.getPos();
        map.at(p) = Block::typeToSymbol(Block::BOMB);
        bombs.push_back(Bomb(p));
    }
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
