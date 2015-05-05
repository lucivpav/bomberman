#ifndef GAME_H
#define GAME_H

#include "map.h"
#include "block.h"
#include "bomb.h"
#include "player.h"
#include "ai_player.h"

#include <map>
#include <chrono>

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

    bool canPlantBomb(const Player & player) const;
    void plantTimedBomb(Player & player);
    void bombExplosion(const Bomb & b);
    bool canMovePlayer(const Pos & where) const;
    void movePlayer(Player &p, const Pos &offset);
    const Map & getMap() const;
    const TimedBomb & getTimedBomb(const Pos & p) const;
private:
    Map map;
    Player * player;
    AIPlayer * enemy;

    std::vector<TimedBomb> timedBombs;
    std::vector<Flame> flames;
    std::map<int, Bonus> bonuses;

    bool expired;

    void loop();
    void keyEvent(int key);
    void drawStatus();

    void handleBombs();
    void handleFlames();
    void genFlames(Pos from, const Pos &to);

    std::chrono::milliseconds getTimestamp() const;
};

#endif
