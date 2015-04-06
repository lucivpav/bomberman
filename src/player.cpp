#include "player.h"

Player::Player()
{

}

Player::Player(const Pos &p)
    :pos(p)
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
