#ifndef BOMB_H
#define BOMB_H

#include "pos.h"

class Bomb
{
public:
    Bomb();
    Bomb(const Pos & pos, int radius);
    Pos getPos() const;
    void setPos(const Pos & p);
    int getRadius() const;
    void setRadius(int r);
private:
    int radius;
    Pos pos;
};

class TimedBomb : public Bomb
{
public:
    TimedBomb();
    TimedBomb(const Pos & pos, int radius);
    bool shouldExplode() const;
    void newFrame();
private:
    int counter;
};

class RemoteBomb : public Bomb
{
public:
    RemoteBomb();
    RemoteBomb(const Pos & pos, int radius);
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
