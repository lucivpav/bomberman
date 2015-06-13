#include "enemy.h"

#include "block.h"

Enemy::Enemy(Game * game, const Pos &pos, int lives, int bombs)
    :Player(game, pos, lives, bombs)
{

}

Enemy::~Enemy()
{

}

char Enemy::getSymbol() const
{
    return Block::typeToSymbol(Block::ENEMY);
}
