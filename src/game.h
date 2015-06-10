#ifndef GAME_H
#define GAME_H

#include "map.h"
#include "block.h"
#include "bomb.h"
#include "player.h"
#include "enemy.h"
#include "ghost.h"
#include "trap.h"
#include "server.h"

#include <map>
#include <thread>
#include <mutex>

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
    Game(const std::string & levelPath, bool genTraps,
         const char * address = 0, const char * port = 0);
    ~Game();

    void plantBombAction(Player & player);

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

    static std::chrono::milliseconds getTimestamp();
private:
    bool mIsOnlineGame;
    std::string mAddress;
    std::string mPort;
    Server mServer;
    bool mListetningFinished;
    bool mConnected;
    std::mutex mLock;
    void listenThread();

    std::string mLevelPath;
    Map map;
    Player * player;
    Enemy * enemy;

    std::vector<TimedBomb> timedBombs;
    std::map<Pos, Flame> flames; // virtual
    std::map<int, Bonus> bonuses;
    std::map<Pos, Ghost> mGhosts; // virtual
    std::map<Pos, Trap> mTraps; // virtual

    bool expired;
    bool mShouldGenTraps;

    bool load(const std::string & levelPath);
    void winAction();
    void loseAction();

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
