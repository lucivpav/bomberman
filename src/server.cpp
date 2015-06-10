#include "server.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <ncurses.h>

Server::Server()
    :mThread(0),
      mListenSocket(-1),
      mClientSocket(-1),
      mDisplay(0)
{

}

Server::~Server()
{
    disconnect();
    if ( mThread )
        delete mThread;
}

void Server::setup(const Player &serverPlayer,
                   const Player &clientPlayer,
                   const Map &map)
{
      mServerPlayer = &serverPlayer;
      mClientPlayer = &clientPlayer;
      mMap = &map;
}

bool Server::connect(const char *address, const char *port)
{
    mListenSocket = prepareSrvSocket(address, port);

    if ( mListenSocket == -1 )
        return false;

    struct sockaddr addr;
    socklen_t addrLen = sizeof(addr);

    mSocketLock.lock();
    int tmp = mListenSocket;
    mSocketLock.unlock();
    mClientSocket = accept(tmp, &addr, &addrLen); // waits till someone connects

    if ( mClientSocket == -1 )
        return false;

    mThread = new std::thread(&Server::listen, this);
    mThread->detach();

    return true;
}

void Server::disconnect()
{
    mSocketLock.lock();
    if ( mClientSocket != -1 )
        close(mClientSocket);
    if ( mListenSocket != -1 )
        close(mListenSocket);
    mSocketLock.unlock();
}

void Server::sendMessage(const ServerMessage &message)
{
    send ( mClientSocket, &message, sizeof(message), 0 );
}

// map size, blocks, lives, bombs
void Server::initOnlineGame()
{
    int w = mMap->width(), h = mMap->height();

    mDisplay = new char*[w];
    for ( int i = 0 ; i < w ; i++ )
    {
        mDisplay[i] = new char[h];
        for ( int j = 0 ; j < h ; j++ )
            mDisplay[i][j] = (char)mvinch(j,i);
    }

    /* map dimensions */
    sendMessage(ServerMessage(ServerMessage::Type::MAP_SIZE, (char)w, (char)h));

    /* blocks */
    for ( int i = 0 ; i < w ; i++ )
        for ( int j = 0 ; j < h ; j++ )
        {
            char c = mDisplay[i][j];

            if ( c == Block::typeToSymbol(Block::PLAYER) )
                c = Block::typeToSymbol(Block::ENEMY);
            else if ( c == Block::typeToSymbol(Block::ENEMY) )
                c = Block::typeToSymbol(Block::PLAYER);

            sendMessage(ServerMessage( ServerMessage::Type::BLOCK,
                                       (char)i,
                                       (char)j,
                                        c ));
        }

    /* lives */
    sendMessage(ServerMessage(ServerMessage::Type::SERVER_LIVES,
                              (char)mServerPlayer->getLives()));
    sendMessage(ServerMessage(ServerMessage::Type::CLIENT_LIVES,
                              (char)mClientPlayer->getLives()));

    /* bombs */
    sendMessage(ServerMessage(ServerMessage::Type::SERVER_BOMBS,
                              (char)mServerPlayer->getBombsAvail()));
    sendMessage(ServerMessage(ServerMessage::Type::CLIENT_BOMBS,
                              (char)mClientPlayer->getBombsAvail()));

    /* speed bonus */
    sendMessage(ServerMessage(ServerMessage::Type::CLIENT_SPEED_BONUS,
                              (char)mClientPlayer->hasSpeedBonus()));

    mPrevClientPlayerBombs = mClientPlayer->getBombsAvail();
    mPrevClientPlayerLives = mClientPlayer->getLives();
    mPrevServerPlayerBombs = mServerPlayer->getBombsAvail();
    mPrevServerPlayerBombs = mServerPlayer->getLives();
    mPrevClientSpeedBonus = mClientPlayer->hasSpeedBonus();
}

void Server::update()
{
    /* display */
    for ( int i = 0 ; i < mMap->width() ; i++ )
        for ( int j = 0 ; j < mMap->height() ; j++ )
        {
            char newChar = (char)mvinch(j, i);
            if ( newChar != mDisplay[i][j] )
            {
                if ( newChar == Block::typeToSymbol(Block::PLAYER) )
                    newChar = Block::typeToSymbol(Block::ENEMY);
                else if ( newChar == Block::typeToSymbol(Block::ENEMY) )
                    newChar = Block::typeToSymbol(Block::PLAYER);

                mDisplay[i][j] = newChar;

                sendMessage(ServerMessage( ServerMessage::Type::BLOCK,
                                           (char)i,
                                           (char)j,
                                           (char) mDisplay[i][j] ));
            }
        }

    /* stats */
    int newClientBombs = mClientPlayer->getBombsAvail();
    int newClientLives = mClientPlayer->getLives();
    int newServerBombs = mServerPlayer->getBombsAvail();
    int newServerLives = mServerPlayer->getLives();
    bool newClientSpeedBonus = mClientPlayer->hasSpeedBonus();

    if ( mPrevClientPlayerBombs != newClientBombs )
    {
        mPrevClientPlayerBombs = newClientBombs;
        sendMessage(ServerMessage(ServerMessage::Type::CLIENT_BOMBS,
                                  (char)newClientBombs));
    }

    if ( mPrevClientPlayerLives != newClientLives )
    {
        mPrevClientPlayerLives = newClientLives;
        sendMessage(ServerMessage(ServerMessage::Type::CLIENT_LIVES,
                                  (char)newClientLives));
    }

    if ( mPrevServerPlayerBombs != newServerBombs )
    {
        mPrevServerPlayerBombs = newServerBombs;
        sendMessage(ServerMessage(ServerMessage::Type::SERVER_BOMBS,
                                  (char)newServerBombs));
    }

    if ( mPrevServerPlayerLives != newServerLives )
    {
        mPrevServerPlayerLives = newServerLives;
        sendMessage(ServerMessage(ServerMessage::Type::SERVER_LIVES,
                                  (char)newServerLives));
    }

    if ( mPrevClientSpeedBonus != newClientSpeedBonus )
    {
        mPrevClientSpeedBonus = newClientSpeedBonus;
        sendMessage(ServerMessage(ServerMessage::Type::CLIENT_SPEED_BONUS,
                                  (char)newClientSpeedBonus));
    }
}

bool Server::getMessage(ClientMessage &message)
{
    mMessageLock.lock();

    if ( mMessages.empty() )
    {
        mMessageLock.unlock();
        return false;
    }

    message = *mMessages.back();
    mMessages.pop();

    mMessageLock.unlock();

    return true;
}

int Server::prepareSrvSocket(const char *address, const char *port)
{
    struct addrinfo * ai;

    if ( getaddrinfo ( address, port, NULL, &ai ) != 0 )
        return -1;

    int sock = socket ( ai -> ai_family, SOCK_STREAM, 0 );

    if ( sock == -1 )
    {
        freeaddrinfo ( ai );
        return -1;
    }

    // enable socket reuse
    int on = 1;
    if ( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ) == -1 )
        return -1;

    if ( bind ( sock, ai -> ai_addr, ai -> ai_addrlen ) != 0 )
    {
        close ( sock );
        freeaddrinfo ( ai );
        return -1;
    }

    if ( ::listen ( sock, 10 ) != 0 )
    {
        close ( sock );
        freeaddrinfo ( ai );
        return -1;
    }
    freeaddrinfo ( ai );
    return sock;
}


void Server::listen()
{
    while ( 1 )
    {
        ClientMessage message;
        int len = recv ( mClientSocket, &message, sizeof ( message ), 0 );

        // error or no further data -> finish
        if ( len <= 0 )
            break;

        mMessageLock.lock();
        mMessages.push( new ClientMessage(message) );
        mMessageLock.unlock();
    }

    close(mClientSocket);
}
