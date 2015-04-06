#ifndef PLAYER_H
#define PLAYER_H

#include "pos.h"

class Player
{
public:
    Player();
    Player(const Pos & p);
    Pos getPos() const;
    void setPos(const Pos & pos);
private:
    Pos pos;
};

#endif
