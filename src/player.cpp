#include "player.h"

#include "game.h" // bomb

Player::Player(Game *g, const Pos &pos, int lives, int bombs)
    : game(g),
      pos(pos),
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
    if ( !lives )
        return;
    lives--;
    if ( remoteBombBonus )
        detonateRemoteBombs();
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
    RemoteBomb b(this, pos, bombRadius);
    remoteBombs.push_back(b);
    return true;
}

void Player::detonateRemoteBombs()
{
    for ( const auto & bomb : remoteBombs )
        game->bombExplosion(bomb);
    bombsAvail += remoteBombs.size();
    remoteBombs.clear();
}

const RemoteBomb * Player::getRemoteBomb(const Pos &p) const
{
    for ( auto it = remoteBombs.begin();
          it != remoteBombs.end();
          it++ )
        if ( it->getPos() == p )
            return &*it;
    return 0;
}

bool Player::detonateRemoteBomb(const Pos &p)
{
    for ( auto it = remoteBombs.begin();
          it != remoteBombs.end();
          it++ )
        if ( it->getPos() == p )
        {
            game->bombExplosion(*it);
            bombsAvail++;
            remoteBombs.erase(it);
            return true;
        }
    return false;
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

char Player::getSymbol() const
{
    return Block::typeToSymbol(Block::PLAYER);
}

int Player::getBombRadius() const
{
    return bombRadius;
}

Player::~Player()
{

}
