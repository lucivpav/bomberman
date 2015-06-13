#ifndef BONUS_H
#define BONUS_H

#include "block.h"

/**
 * @brief The Bonus class represents a bonus in a game.
 */
class Bonus
{
public:
    /**
     * @param type A type of the bonus.
     */
    Bonus(Block::Type type);

    /**
     * @brief Returns the type of the bonus.
     * @return The type of the bonus.
     */
    Block::Type type() const;
private:
    Block::Type mType;
};

#endif
