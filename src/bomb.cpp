#include "bomb.h"
#include "player.h"

/* Bomb */

Bomb::Bomb(Player * owner)
    :radius(3),
      pos(Pos(0,0)),
      owner(owner)
{
}

Bomb::Bomb(Player * owner, const Pos &pos, int radius)
    :radius(radius),
      pos(pos),
      owner(owner)
{
}

Pos Bomb::getPos() const
{
    return pos;
}

void Bomb::setPos(const Pos & p)
{
    pos = p;
}

int Bomb::getRadius() const
{
    return radius;
}

void Bomb::setRadius(int r)
{
    radius = r;
}

Player *Bomb::getOwner()
{
    return owner;
}

/* TimedBomb */

TimedBomb::TimedBomb(Player * owner)
    :Bomb(owner),counter(70)
{
}

TimedBomb::TimedBomb(Player * owner, const Pos & pos, int radius)
    :Bomb(owner, pos, radius),
      counter(70)
{
}

bool TimedBomb::shouldExplode() const
{
    return !counter;
}

void TimedBomb::newFrame()
{
    if ( counter ) counter--;
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
    :counter(12),
      pos(pos)
{
}

Pos Flame::getPos() const
{
    return pos;
}

bool Flame::timedOut() const
{
    return !counter;
}

void Flame::newFrame()
{
    if ( counter ) counter--;
}
