#include "enemy.h"

#include "block.h"

Enemy::Enemy(Game * g, const Pos &pos, int lives, int bombs)
    :Player(g, pos, lives, bombs)
{

}

Enemy::~Enemy()
{

}

char Enemy::getSymbol() const
{
    return Block::typeToSymbol(Block::ENEMY);
}
