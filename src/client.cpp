#include "client.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <ncurses.h>
#include <cassert>

#include "game.h"

Client::Client()
    :mCancelListening(false),
      mSocket(-1),
      mTimestamps(0),
      mWidth(-1),
      mHeight(-1)
{

}

Client::~Client()
{
    disconnect();

    if ( mTimestamps )
    {
        for ( int i = 0 ; i < mWidth ; i++ )
            delete [] mTimestamps[i];

        delete [] mTimestamps;
    }
}

bool Client::isConnected()
{
    mSocketLock.lock();
    bool tmp = mSocket != -1;
    mSocketLock.unlock();
    return tmp;
}

bool Client::connect(const char *address, const char *port)
{
    mSocket = prepareCliSocket ( address, port );

    if ( mSocket == -1 )
        return false;

    std::thread t(&Client::listen, this);
    t.detach();

    return true;
}


void Client::disconnect()
{
    mCancelListeningLock.lock();
    mCancelListening = true;
    mCancelListeningLock.unlock();

    // wait till actual closing
    while ( 1 )
    {
        mSocketLock.lock();
        if ( mSocket == -1 )
        {
            mSocketLock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            break;
        }
        mSocketLock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

bool Client::pendingMessages()
{
    mMessageLock.lock();
    bool tmp = !mMessages.empty();
    mMessageLock.unlock();

    return tmp;
}


bool Client::getMessage(ServerMessage &message)
{
    mMessageLock.lock();

    if ( mMessages.empty() )
    {
        mMessageLock.unlock();
        return false;
    }

    message = mMessages.front();
    mMessages.pop_front();

    mMessageLock.unlock();
    return true;
}

void Client::sendMessage(const ClientMessage &message)
{
    send ( mSocket, &message, sizeof(message), 0 );
}

bool Client::initOnlineGame(int & serverLives,
                            int & serverBombs,
                            int & clientLives,
                            int & clientBombs,
                            bool & speedBonus)
{
    clear();
    Countdown timeLimit(5000);

    /* size */
    while ( 1 )
    {

        if ( timeLimit.expired() )
            return false;

        mMessageLock.lock();

        for ( auto it = mMessages.begin();
              it != mMessages.end();
              ++it )
        {
            if ( (*it).type == ServerMessage::Type::MAP_SIZE )
            {
                mWidth = (int) (*it).val[0];
                mHeight = (int) (*it).val[1];
                mTimestamps = new std::chrono::milliseconds*[mWidth];
                for ( int i = 0 ; i < mWidth ; i++ )
                {
                    mTimestamps[i] = new std::chrono::milliseconds[mHeight];
                    for ( int j = 0 ; j < mHeight ; j++ )
                        mTimestamps[i][j] = std::chrono::milliseconds(1);
                }
            }
            mMessages.erase(it);
            break;
        }

        mMessageLock.unlock();

        if ( mWidth != -1 )
            break;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    serverLives = serverBombs = clientLives = clientBombs = -1;
    int blocksReceived = 0;
    bool bonusReceived = false;

    while ( 1 )
    {
        if ( timeLimit.expired() )
            return false;

        if ( blocksReceived == mWidth * mHeight &&
             serverLives != -1 &&
             serverBombs != -1 &&
             clientLives != -1 &&
             clientBombs != -1 &&
             bonusReceived )
            break;

        ServerMessage m;
        if ( getMessage(m) )
        {
            if ( m.type == ServerMessage::Type::BLOCK )
            {
                int x = (int) m.val[0];
                int y = (int) m.val[1];

                if ( mTimestamps[x][y] != std::chrono::milliseconds(1) ) // already received
                    continue;

                mvaddch(y, x, m.val[2]);
                mTimestamps[x][y] = Game::getTimestamp();
                blocksReceived++;
            }
            else if ( m.type == ServerMessage::Type::CLIENT_BOMBS )
            {
                clientBombs = (int) m.val[0];
            }
            else if ( m.type == ServerMessage::Type::CLIENT_LIVES )
            {
                clientLives = (int) m.val[0];
            }
            else if ( m.type == ServerMessage::Type::SERVER_BOMBS )
            {
                serverBombs = (int) m.val[0];
            }
            else if ( m.type == ServerMessage::Type::SERVER_LIVES )
            {
                serverLives = (int) m.val[0];
            }
            else if ( m.type == ServerMessage::Type::CLIENT_SPEED_BONUS )
            {
                speedBonus = (bool) m.val[0];
                bonusReceived = true;
            }
        }
    }

    return true;
}

void Client::update(int &serverLives,
                    int &serverBombs,
                    int &clientLives,
                    int &clientBombs,
                    bool &speedBonus)
{
    if ( !isConnected() )
        return;

    const int limit = 200; // per frame

    for ( int i = 0 ; i < limit ; i++ )
    {
        ServerMessage m;
        if ( !getMessage(m) )
            break;

        if ( m.type == ServerMessage::Type::BLOCK )
        {
            int x = (int) m.val[0];
            int y = (int) m.val[1];

            std::chrono::milliseconds newStamp = Game::getTimestamp();

            // ignore old msg
            if ( mTimestamps[x][y] > newStamp )
                continue;

            mvaddch(y, x, m.val[2]);
            mTimestamps[x][y] = newStamp;
        }
        else if ( m.type == ServerMessage::Type::CLIENT_BOMBS )
        {
            clientBombs = (int) m.val[0];
        }
        else if ( m.type == ServerMessage::Type::CLIENT_LIVES )
        {
            clientLives = (int) m.val[0];
        }
        else if ( m.type == ServerMessage::Type::SERVER_BOMBS )
        {
            serverBombs = (int) m.val[0];
        }
        else if ( m.type == ServerMessage::Type::SERVER_LIVES )
        {
            serverLives = (int) m.val[0];
        }
        else if ( m.type == ServerMessage::Type::CLIENT_SPEED_BONUS )
        {
            speedBonus = (bool) m.val[0];
        }

    }
}

void Client::closeSockets()
{
    mSocketLock.lock();

    if ( mSocket != -1 )
        close ( mSocket );
    mSocket = -1;

    mSocketLock.unlock();
}

/**
 * !!! WARNING !!! ACHTUNG !!! ATTENZIONE !!!
 * This method has been based on the following code samples:
 * https://edux.fit.cvut.cz/courses/BI-PA2/_media/net_2015.tgz
 */
int Client::prepareCliSocket(const char *address, const char *port)
{
    struct addrinfo * ai;

    if ( getaddrinfo ( address, port, NULL, &ai ) != 0 ) return -1;

    int sock = socket ( ai -> ai_family, SOCK_STREAM, 0 );
    if ( sock == -1 )
    {
        freeaddrinfo ( ai );
        return -1;
    }

    if ( ::connect ( sock, ai -> ai_addr, ai -> ai_addrlen ) != 0 )
    {
        close ( sock );
        freeaddrinfo ( ai );
        return -1;
    }
    freeaddrinfo ( ai );
    return sock;
}

/**
 * !!! WARNING !!! ACHTUNG !!! ATTENZIONE !!!
 * This method has been based on the following code samples:
 * https://edux.fit.cvut.cz/courses/BI-PA2/_media/net_2015.tgz
 */
void Client::listen()
{
    struct timeval tv;
    fd_set set;

    while ( 1 )
    {
        mCancelListeningLock.lock();
        if ( mCancelListening )
        {
            mCancelListeningLock.unlock();
            break;
        }
        mCancelListeningLock.unlock();

        tv.tv_sec = 0;
        tv.tv_usec = 500000;

        FD_ZERO(&set);
        FD_SET(mSocket, &set);

        if ( select(mSocket+1, &set, NULL, NULL, &tv) == -1 )
        {
            assert ( false );
        }
        if ( FD_ISSET(mSocket, &set) )
        {
            ServerMessage m;
            int len = recv ( mSocket, &m, sizeof (m), 0 );

            // error or no further data -> finish
            if ( len <= 0 )
                break;

            mMessageLock.lock();
            mMessages.push_back( m );
            mMessageLock.unlock();
        }
    }

    FD_CLR(mSocket, &set);

    closeSockets();
}
