#ifndef PLAYER_H
#define PLAYER_H

#include "pos.h"
#include "bomb.h"

#include <vector>

class Bomb;
class Game;

/**
 * @brief The Player class represents a player that controls a bomberman.
 */
class Player
{
public:
    /**
     * @param game A pointer to the Game.
     * @param pos Initial pos.
     * @param lives Initial lives.
     * @param bombs Initial bombs.
     */
    Player(Game * game, const Pos & pos, int lives, int bombs);

    virtual ~Player();

    /**
     * @brief Returns a position of the Player.
     * @return The position of the Player.
     */
    Pos getPos() const;

    /**
     * @brief Changes the position of the Player.
     * @param pos The new position of the Player.
     */
    void setPos(const Pos & pos);

    /**
     * @brief Substracts one life from Player's lives (if possible).
     */
    void die();

    /**
     * @brief Checks whether the Player is dead, i.e. has zero lives.
     * @return true if the Player has zero lives, false otherwise.
     */
    bool isDead() const;

    /**
     * @brief Makes the Player plant a Bomb if possible.
     * @param bomb The Bomb to be planted.
     * @return true if the bomb has been planted, false otherwise.
     */
    bool plantBomb(Bomb &bomb);

    /**
     * @brief Makes the Player plant a RemoteBomb if possible.
     * @return true if the bomb has been planted, false otherwise.
     */
    bool plantRemoteBomb();

    /**
     * @brief Checks whether the Player has any RemoteBomb on the game board.
     * @return true if the Player has any RemoteBomb on the game board, false
     * otherwise.
     */
    bool hasRemoteBombOnMap() const;

    /**
     * @brief Detonates all Player's remote bomb (if any).
     */
    void detonateRemoteBombs();

    /**
     * @brief Returns a RemoteBomb at given position if any.
     * @return A RemoteBomb at given position, 0 otherwise.
     */
    const RemoteBomb * getRemoteBomb(const Pos & pos) const;

    /**
     * @brief Detonates RemoteBomb the Player owns at given position (if any).
     * @return true if the bomb has been detonated, false otherwise.
     */
    bool detonateRemoteBomb(const Pos & p);

    /**
     * @brief Sets up the remote bomb bonus for the Player.
     * @param enable true to enable the bonus, false to disable.
     */
    void setRemoteBombBonus(bool enable);

    /**
     * @brief Checks whether the Player has a remote bomb bonus.
     * @return true if the Player has a remote bomb bonus, false otherwise.
     */
    bool hasRemoteBombBonus() const;

    /**
     * @brief Adds one bomb to Player's disposal (if within limit).
     */
    void addBomb();

    /**
     * @brief Returns the current number of lives.
     * @return The current number of lives.
     */
    int getLives() const;

    /**
     * @brief Returns the current number of bombs available.
     * @return The current number of bombs available.
     */
    int getBombsAvail() const;

    /**
     * @brief Returns the radius of bomb explosions Player's bombs can cause.
     * @return The radius of bomb explosions Player's bombs can cause.
     */
    int getBombRadius() const;

    /**
     * @brief Implifies the radius of Player's bomb's explosions by one
     * (if within limit).
     */
    void upgradeBombRadius();

    /**
     * @brief Changes the radius of bomb explosions Player's bombs can cause
     * to default value.
     */
    void defaultBombRadius();

    /**
     * @brief Sets up the speed bonus for the Player.
     * @param enable true to enable, false to disable.
     */
    void setSpeedBonus(bool enable);

    /**
     * @brief Checks whether the Player has a speed bonus.
     * @return true if the Player has a speed bonus, false otherwise.
     */
    bool hasSpeedBonus() const;

    /**
     * @brief Returns the symbol, a graphical representation of an object in
     * the game, associated with the Player.
     * @return The symbol associated with the Player.
     */
    virtual char getSymbol() const;
protected:
    /**
     * @brief mGame A pointer to the Game.
     */
    Game * mGame;
private:
    Pos mPos;
    int mLives;
    int mMaxBombs;
    int mBombsAvail;
    int mBombRadius;
    bool mSpeedBonus;
    bool mRemoteBombBonus;

    std::vector<RemoteBomb> mRemoteBombs;
};

#endif
