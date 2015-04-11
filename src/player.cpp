#include "player.h"

#include "game.h" // bomb

Player::Player(const Pos &pos, int lives, int bombs)
    : pos(pos),
      lives(lives),
      maxBombs(bombs),
      bombsAvail(bombs),
      bombRadius(3)
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

Bomb Player::plantBomb(bool & success)
{
    if ( !bombsAvail )
        success = false;
    bombsAvail--;
    success = true;
    return Bomb(pos, bombRadius);
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

void Player::upgradeBombRadius()
{
    if ( bombRadius < 100 )
        bombRadius++;
}

void Player::defaultBombRadius()
{
    bombRadius = 3;
}
