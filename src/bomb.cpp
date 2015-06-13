#include "bomb.h"
#include "player.h"

/* Bomb */

Bomb::Bomb(Player * owner)
    :mRadius(3),
      mPos(Pos(0,0)),
      mOwner(owner)
{
}

Bomb::Bomb(Player * owner, const Pos &pos, int radius)
    :mRadius(radius),
      mPos(pos),
      mOwner(owner)
{
}

Pos Bomb::getPos() const
{
    return mPos;
}

void Bomb::setPos(const Pos & pos)
{
    mPos = pos;
}

int Bomb::getRadius() const
{
    return mRadius;
}

void Bomb::setRadius(int radius)
{
    mRadius = radius;
}

Player *Bomb::getOwner()
{
    return mOwner;
}

/* TimedBomb */

TimedBomb::TimedBomb(Player * owner)
    :Bomb(owner),
      mExpired(2300)
{
}

TimedBomb::TimedBomb(Player * owner, const Pos & pos, int radius)
    :Bomb(owner, pos, radius),
      mExpired(2300)
{
}

bool TimedBomb::expired()
{
    return mExpired.expired();
}

/* RemoteBomb */

RemoteBomb::RemoteBomb(Player * owner)
    :Bomb(owner)
{
}

RemoteBomb::RemoteBomb(Player * owner, const Pos &pos, int radius)
    :Bomb(owner, pos, radius)
{
}

/* Flame */
Flame::Flame(const Pos & pos)
    :mExpired(400),
      mPos(pos)
{
}

Pos Flame::getPos() const
{
    return mPos;
}

bool Flame::expired()
{
    return mExpired.expired();
}
