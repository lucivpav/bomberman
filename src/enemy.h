#ifndef ENEMY_H
#define ENEMY_H

#include "player.h"

/**
 * @brief The Enemy class describes a general enemy player.
 */
class Enemy : public Player
{
public:
    /**
     * @param game A pointer to the Game.
     * @param pos Initial position.
     * @param lives Initial lives.
     * @param bombs Initial bombs.
     */
    Enemy(Game * game, const Pos & pos, int lives, int bombs);

    virtual ~Enemy();

    /**
     * @brief Instructs the Enemy to make decision.
     */
    virtual void makeDecision() = 0;

    virtual char getSymbol() const;
};

#endif
