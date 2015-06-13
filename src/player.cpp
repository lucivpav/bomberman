#include "player.h"

#include "game.h" // bomb

Player::Player(Game *game, const Pos &pos, int lives, int bombs)
    : mGame(game),
      mPos(pos),
      mLives(lives),
      mMaxBombs(bombs),
      mBombsAvail(bombs),
      mBombRadius(3),
      mSpeedBonus(false),
      mRemoteBombBonus(false)
{
}

Pos Player::getPos() const
{
    return mPos;
}

void Player::setPos(const Pos &pos)
{
    this->mPos = pos;
}

void Player::die()
{
    if ( !mLives )
        return;
    mLives--;
    if ( mRemoteBombBonus )
        detonateRemoteBombs();
}

bool Player::isDead() const
{
    return !mLives;
}

bool Player::plantBomb(Bomb & bomb)
{
    if ( !mBombsAvail )
        return false;
    mBombsAvail--;
    bomb.setPos(mPos);
    bomb.setRadius(mBombRadius);
    return true;
}

bool Player::plantRemoteBomb()
{
    if ( !mRemoteBombBonus || !mGame->canPlantBomb(*this) )
        return false;
    mBombsAvail--;
    RemoteBomb b(this, mPos, mBombRadius);
    mRemoteBombs.push_back(b);
    mGame->getMap().at(getPos()) = Block::typeToSymbol(Block::REMOTE_BOMB);
    return true;
}

bool Player::hasRemoteBombOnMap() const
{
    return !mRemoteBombs.empty();
}

void Player::detonateRemoteBombs()
{
    if ( !hasRemoteBombBonus() )
        return;

    while ( !mRemoteBombs.empty() )
    {
        Bomb b = mRemoteBombs.back();
        mRemoteBombs.pop_back();
        mGame->bombExplosion(b); // this may detonate own bombs
        mBombsAvail++;
    }
}

const RemoteBomb * Player::getRemoteBomb(const Pos &pos) const
{
    for ( auto it = mRemoteBombs.begin();
          it != mRemoteBombs.end();
          it++ )
        if ( it->getPos() == pos )
            return &*it;
    return 0;
}

bool Player::detonateRemoteBomb(const Pos &p)
{
    for ( auto it = mRemoteBombs.begin();
          it != mRemoteBombs.end();
          it++ )
        if ( it->getPos() == p )
        {
            Bomb b = *it;
            mRemoteBombs.erase(it);
            mGame->bombExplosion(b); // this may detonate own bombs ( even self )
            mBombsAvail++;
            return true;
        }
    return false;
}

void Player::setRemoteBombBonus(bool enable)
{
    mRemoteBombBonus = enable;
}

bool Player::hasRemoteBombBonus() const
{
    return mRemoteBombBonus;
}

void Player::addBomb()
{
    if ( mBombsAvail == mMaxBombs )
    {
        if ( mMaxBombs >= 4 )
            return;
        mMaxBombs++;
    }
    mBombsAvail++;
}

int Player::getLives() const
{
    return mLives;
}

int Player::getBombsAvail() const
{
    return mBombsAvail;
}

void Player::upgradeBombRadius()
{
    if ( mBombRadius < 10 )
        mBombRadius++;
}

void Player::defaultBombRadius()
{
    mBombRadius = 3;
}

void Player::setSpeedBonus(bool enable)
{
    mSpeedBonus = enable;
}

bool Player::hasSpeedBonus() const
{
    return mSpeedBonus;
}

char Player::getSymbol() const
{
    return Block::typeToSymbol(Block::PLAYER);
}

int Player::getBombRadius() const
{
    return mBombRadius;
}

Player::~Player()
{

}
