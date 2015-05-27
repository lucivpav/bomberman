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
    bool makeDecision(); /* returns false when dies */
private:
    Game * mGame;
    Pos mPos;
    Pos mDirection;
    int mCountdown; /* direction change countdown */

    /**
     * @brief changeDirection
     * @return false if no direction change leads to ability to move
     */
    bool changeDirection();
};

#endif
