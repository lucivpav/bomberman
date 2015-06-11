#ifndef CLIENTGAME_H
#define CLIENTGAME_H

#include "client.h"

class ClientGame
{
public:
    ClientGame(const char * address,
               const char * port);
private:
    Client mClient;
    std::string mAddress;
    std::string mPort;

    int mLives;
    int mBombs;
    bool mSpeedBonus;

    int mEnemyLives;
    int mEnemyBombs;

    bool mExpired;

    void loop();
    void keyEvent(int key);
    void draw();

    void drawStatus() const;

    void winAction();
    void loseAction();
    void networkErrorAction();
};

#endif
