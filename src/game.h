#ifndef GAME_H
#define GAME_H

#include "map.h"

class Bomb
{
public:
    Bomb(const Pos & pos):counter(90),pos(pos){}
    bool shouldExplode() const { return !counter; }
    void newFrame() { if ( counter ) counter--; }
    Pos getPos() const { return pos; }
    void setPos(const Pos & pos) { this->pos = pos; }
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
    Player player;
    std::vector<Bomb> bombs;

    void loop();
    void keyEvent(int key);

    void movePlayer(Player &p, const Pos &offset);
};

#endif
