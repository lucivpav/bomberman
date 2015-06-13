#ifndef BOMB_H
#define BOMB_H

#include "pos.h"
#include "countdown.h"

class Player;

/**
 * @brief The Bomb class represents a general bomb.
 */
class Bomb
{
public:
    /**
     * @param owner Owner of the bomb.
     */
    Bomb(Player * owner);

    /**
     * @param owner Owner of the bomb.
     * @param pos Position of the bomb.
     * @param radius Radius of the bomb explosion.
     */
    Bomb(Player * owner, const Pos & pos, int radius);

    /**
     * @brief Returns a position of the bomb.
     * @return A position of the bomb.
     */
    Pos getPos() const;

    /**
     * @brief Changes position of the bomb.
     * @param pos New position of the bomb.
     */
    void setPos(const Pos & pos);

    /**
     * @brief Returns radius of the bomb explostion.
     * @return Radius of the bomb explosion.
     */
    int getRadius() const;

    /**
     * @brief Changes radius of the bomb explosion.
     * @param radius New radius of the bomb explosion.
     */
    void setRadius(int radius);

    /**
     * @brief Returns the owner of the bomb.
     * @return The owner of the bomb.
     */
    Player * getOwner();
private:
    int mRadius;
    Pos mPos;
    Player * mOwner;
};

/**
 * @brief The TimedBomb class represents a bomb that
 *  explodes after a period of time.
 */
class TimedBomb : public Bomb
{
public:
    /**
     * @param owner Owner of the bomb.
     */
    TimedBomb(Player * owner);

     /**
     * @param owner Owner of the bomb.
     * @param pos Position of the bomb.
     * @param radius Radius of the bomb explosion.
     */
    TimedBomb(Player * owner, const Pos & pos, int radius);

    /**
     * @brief Returns whether the bomb is ready to explode.
     * @return Returns true if the bomb is ready to explode, false otherwise.
     */
    bool expired();
private:
    Countdown mExpired;
};

/**
 * @brief The RemoteBomb class represents a bomb that can be exploded remotely.
 */
class RemoteBomb : public Bomb
{
public:
    /**
     * @param owner Owner of the bomb.
     */
    RemoteBomb(Player * owner);

     /**
     * @param owner Owner of the bomb.
     * @param pos Position of the bomb.
     * @param radius Radius of the bomb explosion.
     */
    RemoteBomb(Player * owner, const Pos & pos, int radius);
};

/**
 * @brief The Flame class represents a signle flame object
 * that comes out of a bomb explosion.
 */
class Flame
{
public:
    /**
     * @param pos Pos of the flame.
     */
    Flame(const Pos & pos);

    /**
     * @brief Returns the position of the flame.
     * @return Position of the flame.
     */
    Pos getPos() const;

    /**
     * @brief Returns whether life of the flame is over.
     * @return true if the life of the flame is over, false otherwise.
     */
    bool expired();
private:
    Countdown mExpired;
    Pos mPos;
};

#endif
