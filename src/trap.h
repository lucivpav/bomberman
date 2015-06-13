#ifndef TRAP_H
#define TRAP_H

#include "pos.h"
#include "countdown.h"

class Map;

/**
 * @brief The Trap class represents a trap in a game. The trap can be
 * walked on. The Trap may be closed or open. If the trap is open, it
 * causes invocation of Player::die() on a Player that steps on it.
 * If the Trap is closed, it causes no harm.
 */
class Trap
{
public:
    /**
     * @param map The Map associated with the Game.
     * @param pos Position of the Trap.
     */
    Trap(Map & map, const Pos & pos);

    /**
     * @brief Returns the position of the Trap.
     * @return The position of the Trap.
     */
    Pos getPos() const;

    /**
     * @brief Updates the trap's inner state. E.g. it may decide
     * to close or open. This method shall be called every frame.
     */
    void update();

    /**
     * @brief Checks whether the Trap is open.
     * @return true if the Trap is open, false otherwise.
     */
    bool isOpen() const;
private:
    static Map * mMap;

    Pos mPos;
    bool mOpened;
    Countdown mCountdown; /* change mode countdown */
};

#endif
