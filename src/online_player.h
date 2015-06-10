#ifndef ONLINE_PLAYER_H
#define ONLINE_PLAYER_H

#include "enemy.h"
#include "server.h"

class OnlinePlayer : public Enemy
{
public:
    OnlinePlayer(Game * g,
                 const Pos & pos,
                 int lives,
                 int bombs,
                 Server * s);
    virtual ~OnlinePlayer();

    virtual void makeDecision();
private:
    Server * mServer;
};

#endif
