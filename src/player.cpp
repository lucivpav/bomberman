#include "player.h"

Player::Player(const Pos &pos, int lives, int bombs)
    : pos(pos),
      lives(lives),
      maxBombs(bombs),
      bombsAvail(bombs)
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


void Player::die()
{
    if ( lives )
        lives--;
}

bool Player::plantBomb()
{
    if ( !bombsAvail )
        return false;
    bombsAvail--;
    return true;
}

void Player::addBomb()
{
    if ( bombsAvail == maxBombs )
        maxBombs++;
    bombsAvail++;
}

int Player::getLives() const
{
    return lives;
}

int Player::getBombsAvail() const
{
    return bombsAvail;
}
