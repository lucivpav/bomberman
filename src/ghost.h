#ifndef GHOST_H
#define GHOST_H

#include "pos.h"

class Game;

/* todo:
 * ghost and player/enemy overlap
 */
class Ghost
{
public:
    Ghost(Game * g, const Pos & pos, char ground);
    void setPos(const Pos & pos);
    const Pos & getPos() const;
    void setGround(char ground);
    char getGround() const;
    void makeDecision();
private:
    Game * mGame;
    Pos mPos;
    Pos mDirection;
    int mCountdown; /* direction change countdown */
    char mGround; /* the symbol ghost is standing on */

    void changeDirection();
};

#endif
