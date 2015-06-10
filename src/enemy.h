#ifndef ENEMY_H
#define ENEMY_H

#include "player.h"

class Enemy : public Player
{
public:
    Enemy(Game * g, const Pos & pos, int lives, int bombs);
    virtual ~Enemy();

    virtual void makeDecision() = 0;
    virtual char getSymbol() const;
};

#endif
