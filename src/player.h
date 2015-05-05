#ifndef PLAYER_H
#define PLAYER_H

#include "pos.h"
#include "bomb.h"

#include <vector>

class Bomb;
class Game;

class Player
{
public:
    Player(Game * g, const Pos & pos, int lives, int bombs);
    virtual ~Player();
    Pos getPos() const;
    void setPos(const Pos & pos);
    void die();

    bool plantBomb(Bomb &b); /* returns false if no bombs left */
    bool plantRemoteBomb();
    void detonateRemoteBombs();
    const RemoteBomb * getRemoteBomb(const Pos & p) const;
    bool detonateRemoteBomb(const Pos & p);
    void setRemoteBombBonus(bool enable);
    bool hasRemoteBombBonus() const;
    void addBomb();

    int getLives() const;
    int getBombsAvail() const;
    int getBombRadius() const;
    void upgradeBombRadius();
    void defaultBombRadius();
    void setSpeedBonus(bool enable);
    bool hasSpeedBonus() const;

    virtual char getSymbol() const;
protected:
    Game * game;
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
