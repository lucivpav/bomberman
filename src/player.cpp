#include "player.h"

#include "game.h" // bomb

Player::Player(const Pos &pos, int lives, int bombs)
    : pos(pos),
      lives(lives),
      maxBombs(bombs),
      bombsAvail(bombs),
      bombRadius(3),
      speedBonus(false),
      remoteBombBonus(false)
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

bool Player::plantBomb(Bomb & b)
{
    if ( !bombsAvail )
        return false;
    bombsAvail--;
    b.setPos(pos);
    b.setRadius(bombRadius);
    return true;
}

bool Player::plantRemoteBomb()
{
    if ( !bombsAvail || !remoteBombBonus )
        return false;
    bombsAvail--;
    RemoteBomb b(pos, bombRadius);
    remoteBombs.push_back(b);
    return true;
}

std::vector<RemoteBomb> Player::detonateRemoteBombs()
{
    std::vector<RemoteBomb> toReturn = remoteBombs;
    bombsAvail += remoteBombs.size();
    remoteBombs.clear();
    return toReturn;
}

void Player::setRemoteBombBonus(bool enable)
{
    remoteBombBonus = enable;
}

bool Player::hasRemoteBombBonus() const
{
    return remoteBombBonus;
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

void Player::setSpeedBonus(bool enable)
{
    speedBonus = enable;
}

bool Player::hasSpeedBonus() const
{
    return speedBonus;
}
