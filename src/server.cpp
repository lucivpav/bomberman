#include "server.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <ncurses.h>
#include <cassert>

Server::Server()
    :mCancelListening(false),
      mListeningFinished(false),
      mListenSocket(-1),
      mClientSocket(-1),
      mDisplay(0)
{

}

Server::~Server()
{
    disconnect();

    if ( mDisplay )
    {
        for ( int i = 0 ; i < mMap->width() ; i++ )
            delete [] mDisplay[i];

        delete [] mDisplay;
    }

    for ( const auto & m : mMessages )
        delete m;
}

void Server::setup(const Player &serverPlayer,
                   const Player &clientPlayer,
                   const Map &map)
{
      mServerPlayer = &serverPlayer;
      mClientPlayer = &clientPlayer;
      mMap = &map;
}

bool Server::isConnected()
{//cannot be const due to locks
    mSocketLock.lock();
    bool tmp = mClientSocket != -1;
    mSocketLock.unlock();
    return tmp;
}

void Server::listen(const char *address, const char *port)
{
    std::thread t(&Server::listenThread, this, address, port);
    t.detach();
}

bool Server::listeningFinished()
{
    mListeningFinishedLock.lock();
    bool tmp = mListeningFinished;
    mListeningFinishedLock.unlock();
    return tmp;
}

void Server::disconnect()
{
    mCancelListeningLock.lock();
    mCancelListening = true;
    mCancelListeningLock.unlock();

    // wait till sockets get closed
    // poor man's way of waiting for detached thread to finish
    // todo: improve
    while ( 1 )
    {
        mSocketLock.lock();
        if ( mClientSocket == -1 && mListenSocket == -1 )
        {
            mSocketLock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            break;
        }
        mSocketLock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

void Server::sendMessage(const ServerMessage &message)
{
    bool tmp = isConnected();
    if ( !tmp )
        return;

    mSocketLock.lock();

    send ( mClientSocket, &message, sizeof(message), 0 ); // <-- SIGPIPE

    mSocketLock.unlock();
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
    mPrevServerPlayerLives = mServerPlayer->getLives();
    mPrevClientSpeedBonus = mClientPlayer->hasSpeedBonus();
}

void Server::update()
{
    if ( !isConnected() )
        return;

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

void Server::listenThread(const char *address, const char *port)
{
    mListenSocket = prepareSrvSocket(address, port);

    if ( mListenSocket == -1 )
    {
        mListeningFinishedLock.lock();
        mListeningFinished = true;
        mListeningFinishedLock.unlock();

        closeSockets();

        return;
    }

    struct sockaddr addr;
    socklen_t addrLen = sizeof(addr);

    /* listen */
    struct timeval tv;
    fd_set set;

    while ( 1 )
    {
        mCancelListeningLock.lock();
        if ( mCancelListening )
        {
            FD_CLR(mListenSocket, &set);

            closeSockets();

            return;
        }
        mCancelListeningLock.unlock();

        tv.tv_sec = 0;
        tv.tv_usec = 200000;

        FD_ZERO(&set);
        FD_SET(mListenSocket, &set);

        if ( select(mListenSocket+1, &set, NULL, NULL, &tv) == -1 )
        {
            assert ( false );
        }
        if ( FD_ISSET(mListenSocket, &set) )
        {
            mClientSocket = accept(mListenSocket, &addr, &addrLen);
            break;
        }
    }

    FD_CLR(mListenSocket, &set);

    if ( mClientSocket == -1 )
    {
        mListeningFinishedLock.lock();
        mListeningFinished = true;
        mListeningFinishedLock.unlock();

        closeSockets();

        return;
    }

    /* close the listen socket - disable new incoming connections */
    mSocketLock.lock();
    close(mListenSocket);
    mListenSocket = -1;
    mSocketLock.unlock();

    mListeningFinishedLock.lock();
    mListeningFinished = true;
    mListeningFinishedLock.unlock();

    while ( 1 )
    {
        mCancelListeningLock.lock();
        if ( mCancelListening )
        {
            FD_CLR(mClientSocket, &set);
            closeSockets();
            return;
        }
        mCancelListeningLock.unlock();

        tv.tv_sec = 0;
        tv.tv_usec = 30000;

        FD_ZERO(&set);
        FD_SET(mClientSocket, &set);

        if ( select(mClientSocket+1, &set, NULL, NULL, &tv) == -1 )
        {
            assert ( false );
        }
        if ( FD_ISSET(mClientSocket, &set) )
        {
            ClientMessage message;
            int len = recv ( mClientSocket, &message, sizeof ( message ), 0 );

            // error or no further data -> finish
            if ( len <= 0 )
                break;

            mMessageLock.lock();
            mMessages.push_back( new ClientMessage(message) );
            mMessageLock.unlock();
        }
    }

    FD_CLR(mClientSocket, &set);
    closeSockets();
}

void Server::closeSockets()
{
    mSocketLock.lock();
    if ( mClientSocket != -1 )
        close(mClientSocket);
    if ( mListenSocket != -1 )
        close(mListenSocket);

    mClientSocket = -1;
    mListenSocket = -1;

    mSocketLock.unlock();
}

bool Server::getMessage(ClientMessage &message)
{
    mMessageLock.lock();

    if ( mMessages.empty() )
    {
        mMessageLock.unlock();
        return false;
    }

    const auto & m = mMessages.front();
    message = *m;
    delete m;
    mMessages.pop_front();

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
