#ifndef CLIENT_H
#define CLIENT_H

#include <chrono>
#include <deque>
#include <thread>
#include <mutex>

#include "server.h"

struct ServerMessage;

enum class ClientMessage : char { MOVE_LEFT,
                                  MOVE_RIGHT,
                                  MOVE_UP,
                                  MOVE_DOWN,
                                  PLACE_BOMB,
                                  DETONATE_BOMBS };

class Client
{
public:
    Client();
    ~Client();

    bool connect(const char * address,
                 const char * port);
    void disconnect();

    bool getMessage(ServerMessage & message);
    void sendMessage(const ClientMessage & message);

    void initOnlineGame(int & serverLives,
                        int & serverBombs,
                        int & clientLives,
                        int & clientBombs,
                        bool &speedBonus);

    void update(int & serverLives,
                int & serverBombs,
                int & clientLives,
                int & clientBombs,
                bool & speedBonus);
private:
    std::mutex mMessageLock;
    std::deque<ServerMessage*> mMessages;
    std::thread * mThread;
    int mSocket;
    std::chrono::milliseconds ** mTimestamps;
    int mWidth;
    int mHeight;

    int prepareCliSocket(const char * address,
                         const char * port);
    void listen();
};

#endif
