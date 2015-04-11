#ifndef GAME_H
#define GAME_H

#include "map.h"

class Bomb
{
public:
    Bomb(const Pos & pos, int strength):_strength(strength),counter(90),pos(pos){}
    bool shouldExplode() const { return !counter; }
    void newFrame() { if ( counter ) counter--; }
    Pos getPos() const { return pos; }
    void setPos(const Pos & pos) { this->pos = pos; }
    int strength() const { return _strength; }
private:
    int _strength;
    int counter;
    Pos pos;
};

class Flame
{
public:
    Flame(const Pos & pos):counter(15),pos(pos){}
    Pos getPos() const { return pos; }
    bool timedOut() const { return !counter; }
    void newFrame() { if ( counter ) counter--; }
private:
    int counter;
    Pos pos;
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

    void loop();
    void keyEvent(int key);
    void drawStatus();

    void movePlayer(Player &p, const Pos &offset);
    void handleBombs();
    void handleFlames();
    void genFlames(Pos from, const Pos &to);
};

#endif
