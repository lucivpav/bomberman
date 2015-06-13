#ifndef SERVER_H
#define SERVER_H

#include <deque>
#include <thread>
#include <mutex>

#include "client.h"
#include "map.h"
#include "player.h"

enum class ClientMessage : char;

/**
 * @brief The ServerMessage struct represents a message the Server can
 * send to the Client.
 */
struct ServerMessage
{
    /**
     * @brief The Type enum represents a type of the ServerMessage.
     */
    enum class Type : char { MAP_SIZE, ///< format: val[3]: width height unused
                             BLOCK, ///< format: val[3]: x y symbol
                             SERVER_LIVES, ///< format: val[3]: lives unused unused
                             SERVER_BOMBS, ///< format: val[3]: bombs unused unused
                             CLIENT_LIVES, ///< format: val[3]: lives unused unused
                             CLIENT_BOMBS, ///< format: val[3]: bombs unused unused
                             CLIENT_SPEED_BONUS  ///< format: val[3]: enabled unused unused
                           };
    /**
     * @brief The message values are not set. The caller is responsible for
     * setting up the message manually.
     */
    ServerMessage(){}

    /**
     * @brief Creates a message with specific values.
     * @param type Type of the message.
     * @param val1 First item in the val[3] array to be set.
     * @param val2 Second item in the val[3] array to be set.
     * @param val3 Third item in the val[3] array to be set.
     */
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

    /**
     * @brief The type of the message.
     */
    Type type;

    /**
     * @brief The payload of the message.
     */
    char val[3];
};

/**
 * @brief The Server class is responsible for hosting an online game and
 * maintaining a connection to the Client. It makes sure the Client gets
 * all the necessary notifications about the game state through it's
 * update() method.
 */
class Server
{
public:
    /**
     * @brief Prepares the Server for initial set up by setup().
     */
    Server();

    /**
     * @brief Releases all resources in use by Server.
     * It includes calling disconnect(). */
    ~Server();

    /**
     * @brief Prepares the Server for use.
     * @param serverPlayer The Player representing the server side player.
     * @param clientPlayer The Player representing the client side player.
     * @param map The Map associated with the Game.
     */
    void setup(const Player & serverPlayer,
               const Player & clientPlayer,
               const Map & map);

    /**
     * @brief Checks whether the Server is connected to the Client.
     * @return true if the Server is connected to the Client, false otherwise.
     */
    bool isConnected();

    /**
     * @brief Instructs the Server to start listening for incoming connection.
     * This method returns immediately. Call listeningFinished() to check
     * whether the listening process has ended. Then call isConnected() to
     * determine whether the listening process has been successful.
     * @warning Caller is responsible for calling setup() prior calling
     * this method.
     * @param address The address to listen on.
     * @param port The port to listen on.
     */
    void listen(const char * address,
                 const char * port);

    /**
     * @brief Checks whether the listening by listen() method has finished.
     * @return true if the listening by listen() method has finished, false
     * otherwise.
     */
    bool listeningFinished();

    /**
     * @brief Disconnects from the Client. If the Server is not connected to
     * a Client, the method does nothing.
     */
    void disconnect();

    /**
     * @brief Returns the first incoming message in an internal
     * queue. If there is no incoming message, the method return false.
     * The message gets removed from the internal queue after calling
     * this method.
     * @param message Outputs the incoming message.
     * @return true if there was an incoming message, false otherwise.
     */
    bool getMessage(ClientMessage & message);

    /**
     * @brief Call this if the the connection to the
     * Client was successful. This method transfers all the necessary
     * information about the game from the Server, so that the game can
     * be started.
     */
    void initOnlineGame();

    /**
     * @brief Sends all the necessary information about the game state
     * to the Client to keep the Client updated.
     */
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

    void sendMessage(const ServerMessage & message);
};

#endif
