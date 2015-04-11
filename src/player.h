#ifndef PLAYER_H
#define PLAYER_H

#include "pos.h"

class Player
{
public:
    Player(const Pos & pos, int lives, int bombs);
    Pos getPos() const;
    void setPos(const Pos & pos);
    void die();
    bool plantBomb(); /* returns false if no bombs left */
    void addBomb();
private:
    Pos pos;
    int lives;
    int maxBombs;
    int bombsAvail;
};

#endif
