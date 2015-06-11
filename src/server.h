#ifndef SERVER_H
#define SERVER_H

#include <deque>
#include <thread>
#include <mutex>

#include "client.h"
#include "map.h"
#include "player.h"

enum class ClientMessage : char;

struct ServerMessage
{
    enum class Type : char { MAP_SIZE,
                             BLOCK,
                             SERVER_LIVES,
                             SERVER_BOMBS,
                             CLIENT_LIVES,
                             CLIENT_BOMBS,
                             CLIENT_SPEED_BONUS };
    ServerMessage(){}

    ServerMessage(Type type,
                  char val1,
                  char val2 = 0,
                  char val3 = 0)
        : type(type)
    {
        val[0] = val1;
        val[1] = val2;
        val[2] = val3;
    }

    Type type;
    char val[3];
};

class Server
{
public:
    Server();
    ~Server();

    void setup(const Player & serverPlayer,
               const Player & clientPlayer,
               const Map & map);

    bool isConnected();//cannot be const due to locks

    // async
    void listen(const char * address,
                 const char * port);
    bool listeningFinished();

    void disconnect();

    bool getMessage(ClientMessage & message);
    void sendMessage(const ServerMessage & message);

    void initOnlineGame();
    void update();
private:
    void listenThread(const char * address,
                       const char * port);

    void closeSockets();

    bool mCancelListening;
    std::mutex mCancelListeningLock;

    bool mListeningFinished;
    std::mutex mListeningFinishedLock;

    std::mutex mSocketLock;
    std::mutex mMessageLock;
    std::deque<ClientMessage*> mMessages;

    int mListenSocket;
    int mClientSocket;

    int prepareSrvSocket(const char * address,
                         const char * port);
    void listen();

    char ** mDisplay;
    int mPrevServerPlayerLives;
    int mPrevServerPlayerBombs;
    int mPrevClientPlayerLives;
    int mPrevClientPlayerBombs;
    bool mPrevClientSpeedBonus;

    const Player * mServerPlayer;
    const Player * mClientPlayer;
    const Map * mMap;
};

#endif
