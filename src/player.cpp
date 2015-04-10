#include "player.h"

Player::Player()
    :maxBombs(1)
{

}

Player::Player(const Pos &p)
    :pos(p),
      maxBombs(1)
{

}


Pos Player::getPos() const
{
    return pos;
}


void Player::setPos(const Pos &pos)
{
    this->pos = pos;
}
