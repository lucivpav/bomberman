#ifndef BOMB_H
#define BOMB_H

#include "pos.h"

class Player;

class Bomb
{
public:
    Bomb(Player * owner);
    Bomb(Player * owner, const Pos & pos, int radius);
    Pos getPos() const;
    void setPos(const Pos & p);
    int getRadius() const;
    void setRadius(int r);
    Player * getOwner();
private:
    int radius;
    Pos pos;
    Player * owner;
};

class TimedBomb : public Bomb
{
public:
    TimedBomb(Player * owner);
    TimedBomb(Player * owner, const Pos & pos, int radius);
    bool shouldExplode() const;
    void newFrame();
private:
    int counter;
};

class RemoteBomb : public Bomb
{
public:
    RemoteBomb(Player * owner);
    RemoteBomb(Player * owner, const Pos & pos, int radius);
};

class Flame
{
public:
    Flame(const Pos & pos);
    Pos getPos() const;
    bool timedOut() const;
    void newFrame();
private:
    int counter;
    Pos pos;
};

#endif
