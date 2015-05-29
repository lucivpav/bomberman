#ifndef GAME_H
#define GAME_H

#include "map.h"
#include "block.h"
#include "bomb.h"
#include "player.h"
#include "ai_player.h"
#include "ghost.h"
#include "trap.h"

#include <map>

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
    Game(bool genTraps);
    ~Game();

    bool canPlantBomb(const Player & player) const;
    void plantTimedBomb(Player & player);
    void bombExplosion(const Bomb & b);
    bool canMovePlayer(const Pos & where) const;
    void movePlayer(Player &p, const Pos &offset);
    bool canMoveGhost(const Pos & where) const;
    bool moveGhost(Ghost & g, const Pos & offset); /* returns false when dies */
    Map & getMap() ;
    const Bomb * getBomb(const Pos & p) const;
    const TimedBomb & getTimedBomb(const Pos & p) const;
    bool isFlameAt(const Pos & p) const;
    const Trap * trapAt(const Pos & p) const;

    bool withinBounds(const Pos & pos) const;
private:
    Map map;
    Player * player;
    AIPlayer * enemy;

    std::vector<TimedBomb> timedBombs;
    std::map<Pos, Flame> flames; // virtual
    std::map<int, Bonus> bonuses;
    std::map<Pos, Ghost> mGhosts; // virtual
    std::map<Pos, Trap> mTraps; // virtual

    bool expired;

    void loop();
    void keyEvent(int key);
    void drawGhosts() const;
    void drawStatus() const;
    void drawFlames() const;
    void drawTraps() const;

    void handleBombs();
    void handleFlames();
    void genFlames(Pos from, const Pos &to);

    void getCandidates(std::vector<Pos> & out) const;

    void genGhosts(std::vector<Pos> candidates);
    void handleGhosts();

    void genTraps(std::vector<Pos> candidates);
    void handleTraps();

    // debug
    void drawPath() const;
    bool shouldDrawPath;
};

#endif
