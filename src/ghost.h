#ifndef GHOST_H
#define GHOST_H

#include "pos.h"

class Game;

class Ghost
{
public:
    Ghost(Game * g, const Pos & pos);
    void setPos(const Pos & pos);
    const Pos & getPos() const;
    void makeDecision();
private:
    Game * mGame;
    Pos mPos;
    Pos mDirection;
    int mCountdown; /* direction change countdown */
    void changeDirection();
};

#endif
