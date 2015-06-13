#ifndef GHOST_H
#define GHOST_H

#include "pos.h"

class Game;

/**
 * @brief The Ghost class represents a ghost in a Game.
 */
class Ghost
{
public:
    /**
     * @param game A pointer to the Game.
     * @param pos Initial position.
     */
    Ghost(Game * game, const Pos & pos);

    /**
     * @brief Changes the position of the Ghost.
     * @param pos The new position.
     */
    void setPos(const Pos & pos);

    /**
     * @brief Returns the position of the Ghost.
     * @return The position of the Ghost.
     */
    const Pos & getPos() const;

    /**
     * @brief Instructs the Ghost to make decision.
     * @return false if the Ghost dies, true otherwise
     */
    bool makeDecision();
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
