#ifndef ONLINE_PLAYER_H
#define ONLINE_PLAYER_H

#include "enemy.h"
#include "server.h"

/**
 * @brief The OnlinePlayer class represents an Enemy that is controlled by
 * an online player.
 */
class OnlinePlayer : public Enemy
{
public:
    /**
     * @param game A pointer to the Game.
     * @param pos Initial position.
     * @param lives Initial lives.
     * @param bombs Initial bombs.
     * @param server A pointer to the Server.
     */
    OnlinePlayer(Game * game,
                 const Pos & pos,
                 int lives,
                 int bombs,
                 Server * server);

    virtual ~OnlinePlayer();

    virtual void makeDecision();
private:
    Server * mServer;
};

#endif
