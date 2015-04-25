#ifndef GAME_H
#define GAME_H

#include "map.h"
#include "block.h"

#include <map>
#include <chrono>

class Bomb
{
public:
    Bomb():_strength(3),counter(70),pos(0,0){}
    Bomb(const Pos & pos, int strength):_strength(strength),counter(70),pos(pos){}
    bool shouldExplode() const { return !counter; }
    void newFrame() { if ( counter ) counter--; }
    Pos getPos() const { return pos; }
    void setPos(const Pos & pos) { this->pos = pos; }
    int strength() const { return _strength; }
    void setStrength(int radius) { _strength = radius; }
private:
    int _strength;
    int counter;
    Pos pos;
};

class Flame
{
public:
    Flame(const Pos & pos):counter(12),pos(pos){}
    Pos getPos() const { return pos; }
    bool timedOut() const { return !counter; }
    void newFrame() { if ( counter ) counter--; }
private:
    int counter;
    Pos pos;
};

class Bonus
{
public:
    Bonus(Block::Type type):_type(type){}
    Block::Type type() const { return _type; }
    static int key(const Pos & pos) { return (pos.x << 2) + pos.y; }
private:
    Block::Type _type;
};

class Game
{
public:
    Game();
    ~Game();
private:
    Map map;
    Player * player;

    std::vector<Bomb> bombs;
    std::vector<Flame> flames;
    std::map<int, Bonus> bonuses;

    bool expired;

    void loop();
    void keyEvent(int key);
    void drawStatus();

    void movePlayer(Player &p, const Pos &offset);
    void handleBombs();
    void handleFlames();
    void genFlames(Pos from, const Pos &to);

    std::chrono::milliseconds getTimestamp() const;
};

#endif
