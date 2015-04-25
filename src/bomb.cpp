#include "bomb.h"

/* Bomb */

Bomb::Bomb()
    :radius(3),
      pos(Pos(0,0))
{
}

Bomb::Bomb(const Pos &pos, int radius)
    :radius(radius),
      pos(pos)
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

/* TimedBomb */

TimedBomb::TimedBomb()
    :Bomb(),counter(70)
{
}

TimedBomb::TimedBomb(const Pos & pos, int radius)
    :Bomb(pos, radius),
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

RemoteBomb::RemoteBomb()
    :Bomb()
{
}

RemoteBomb::RemoteBomb(const Pos &pos, int radius)
    :Bomb(pos, radius)
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
