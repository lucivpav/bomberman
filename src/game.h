#ifndef GAME_H
#define GAME_H

#include "map.h"
#include "block.h"
#include "bonus.h"
#include "bomb.h"
#include "player.h"
#include "enemy.h"
#include "ghost.h"
#include "trap.h"
#include "server.h"

#include <map>

/**
 * @brief The Game class represents represents a game. This can be either a
 * singleplayer game or a game from a server perspective.
 */
class Game
{
public:
    /**
     * @param levelPath A filesystem path to the folder where levels are saved.
     * @param enableTraps Whether to enable traps in the game.
     * @param enableGhosts Whether to enable ghosts in the game.
     * @param lives The number of lives players get.
     * @param address The address to host a multiplayer game at. Leave default
     * in order to start a singleplayer game.
     * @param port The port to host a multiplayer game at. Leave default
     * in order to start a singleplayer game.
     */
    Game(const std::string & levelPath,
         bool enableTraps = true,
         bool enableGhosts = true,
         int lives = 3,
         const char * address = 0,
         const char * port = 0);

    ~Game();

    /**
     * @brief Makes the player plant a bomb (if possible).
     * @param player The player to place the bomb.
     */
    void plantBombAction(Player & player);

    /**
     * @brief Returns whether the player can plant a bomb.
     * @return true if the player can plant a bomb, false otherwise.
     */
    bool canPlantBomb(const Player & player) const;

    /**
     * @brief Makes the bomb explode.
     * @param bomb The bomb to be exploded.
     */
    void bombExplosion(const Bomb & bomb);

    /**
     * @brief Returns whether a player can move to the given position
     * (presuming the player could teleport)
     * @return true if a player can move to the position, false otherwise.
     */
    bool canMovePlayer(const Pos & where) const;

    /**
     * @brief Moves the player to a new position (if possible).
     * @param player The player to me moved.
     * @param offset
     */
    void movePlayer(Player &player, const Pos &offset);

    /**
     * @brief Returns whether a ghost can move to the given
     * position (presuming the ghost could teleport)
     * @return ture if a ghost can move to the position, false otherwise
     */
    bool canMoveGhost(const Pos & where) const;

    /**
     * @brief Moves the ghost to a new position.
     * @warning The method does not check whether the ghost is
     * actually able to move to the new location. You may want to call
     * canMoveGhost() prior calling this.
     * @param ghost The ghost to be moved.
     * @param offset The 'side vector' to move by.
     * E.g. (1,0) causes the ghost to move right by one block.
     * @return false if the ghost dies, true otherwise.
     */
    bool moveGhost(Ghost & ghost, const Pos & offset);


    /**
     * @brief Returns the Map associated with the Game.
     * @return The Map associated with the Game.
     */
    Map & getMap();

    /**
     * @brief Returns a Bomb at given position.
     * @return A Bomb at given position or 0 otherwise.
     */
    const Bomb * getBomb(const Pos & pos) const;

    /**
     * @brief Returns a TimedBomb at given position.
     * @warning The method does not check whether there actually is a TimedBomb
     * at given location. It's caller's responsibility to make sure of that.
     * @return A TimedBomb at given position.
     */
    const TimedBomb & getTimedBomb(const Pos & pos) const;

    /**
     * @brief Returns true if there is a Flame at given position.
     * @return true if there is a Flame at given position, false otherwise.
     */
    bool isFlameAt(const Pos & pos) const;


    /**
     * @brief Returns a Trap at given position.
     * @return A trap at given position, 0 otherwise.
     */
    const Trap * trapAt(const Pos & p) const;

    /**
     * @brief Returns current time in milliseconds since epoch.
     * @return Current time in milliseconds since epoch.
     */
    static std::chrono::milliseconds getTimestamp();
private:
    bool mIsOnlineGame;
    std::string mAddress;
    std::string mPort;
    Server mServer;

    /* setup */
    std::string mLevelPath;
    bool mEnableTraps;
    bool mEnableGhosts;
    int mLives;

    Map mMap;
    Player * mPlayer;
    Enemy * mEnemy;

    std::vector<TimedBomb> mTimedBombs;
    std::map<Pos, Flame> mFlames; // virtual
    std::map<int, Bonus> mBonuses;
    std::map<Pos, Ghost> mGhosts; // virtual
    std::map<Pos, Trap> mTraps; // virtual

    bool mExpired;

    bool load(const std::string & levelPath,
              bool enableTraps,
              bool enableGhosts,
              int lives);

    void winAction();
    void loseAction();
    void networkErrorAction();

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

    int posKey(const Pos & pos); // uniq identifier for a Pos
    void plantTimedBomb(Player & player);
    bool initOnlineGame();

#ifdef DEBUG
    void drawPath() const;
    bool mShouldDrawPath;
#endif
};

#endif
