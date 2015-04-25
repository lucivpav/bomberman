#ifndef PLAYER_H
#define PLAYER_H

#include "pos.h"
#include "bomb.h"

#include <vector>

class Bomb;

class Player
{
public:
    Player(const Pos & pos, int lives, int bombs);
    Pos getPos() const;
    void setPos(const Pos & pos);
    void die();

    bool plantBomb(Bomb &b); /* returns false if no bombs left */
    bool plantRemoteBomb();
    std::vector<RemoteBomb> detonateRemoteBombs();
    void setRemoteBombBonus(bool enable);
    bool hasRemoteBombBonus() const;
    void addBomb();

    int getLives() const;
    int getBombsAvail() const;
    void upgradeBombRadius();
    void defaultBombRadius();
    void setSpeedBonus(bool enable);
    bool hasSpeedBonus() const;
private:
    Pos pos;
    int lives;
    int maxBombs;
    int bombsAvail;
    int bombRadius;
    bool speedBonus;
    bool remoteBombBonus;

    std::vector<RemoteBomb> remoteBombs;
};

#endif
