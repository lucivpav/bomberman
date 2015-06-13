#ifndef CLIENTGAME_H
#define CLIENTGAME_H

#include "client.h"

/**
 * @brief The ClientGame class represents an online game from the client side.
 */
class ClientGame
{
public:
    /**
     * @brief ClientGame Starts an online game from the client perspective.
     * @param address The address to connect to.
     * @param port The port to connect to.
     */
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

    void drawStatus() const;

    void winAction();
    void loseAction();
    void networkErrorAction();
};

#endif
