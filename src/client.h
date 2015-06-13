#ifndef CLIENT_H
#define CLIENT_H

#include <chrono>
#include <deque>
#include <thread>
#include <mutex>

#include "server.h"

struct ServerMessage;

/**
 * @brief The ClientMessage enum represents a type of message the
 * Client can send to the Server.
 */
enum class ClientMessage : char { MOVE_LEFT,
                                  MOVE_RIGHT,
                                  MOVE_UP,
                                  MOVE_DOWN,
                                  PLACE_BOMB,
                                  DETONATE_BOMBS };

/**
 * @brief The Client class is responsible for maintaining and establishing
 * a connection to the Server. It draws and updates
 * the map on the screen through it's update() method.
 */
class Client
{
public:
    /**
     * @brief Prepares the Client for use.
     */
    Client();

    /**
     * @brief Releases all resources in use by Client.
     * It includes calling disconnect().
     */
    ~Client();

    /**
     * @brief Returns whether the Client is connected to the Server.
     * @return true if the Client is connected to the Server, false otherwise.
     */
    bool isConnected();

    /**
     * @brief Connects to a Server.
     * @param address The address to connect to.
     * @param port The port to connect to.
     * @return true if the Client was connected to the Server, false otherwise.
     */
    bool connect(const char * address,
                 const char * port);

    /**
     * @brief Disconnects from the Server. If the Client is not connected to a
     * Server, the method does nothing.
     */
    void disconnect();

    /**
     * @brief Returns whether there are any incoming messages that have
     *  not been processed yet.
     * @return true if there are any unprocessed messages, false otherwise.
     */
    bool pendingMessages();

    /**
     * @brief Returns the first incoming message in an internal
     * queue. If there is no incoming message, the method return false.
     * The message gets removed from the internal queue after calling
     * this method.
     * @param message Outputs the incoming message.
     * @return true if there was an incoming message, false otherwise.
     */
    bool getMessage(ServerMessage & message);

    /**
     * @brief Sends a message to the Server.
     * @param message The message to be send.
     */
    void sendMessage(const ClientMessage & message);

    /**
     * @brief Call this if the the connection to the
     * Server was successful. This method effectively waits for all the
     * necessary information about the game from the Server, so that the game can
     * be started. If the initialization fails, the method returns false.
     * @param serverLives Outputs the lives the server player has.
     * @param serverBombs Outputs the number of bombs the server player has.
     * @param clientLives Outputs the lives the client player has.
     * @param clientBombs Outputs the number of bombs the client player has.
     * @param speedBonus Outputs true if the client player has a speed bonus,
     *  false otherwise.
     * @return true if the game was initialized succesfully, false on failure.
     */
    bool initOnlineGame(int & serverLives,
                        int & serverBombs,
                        int & clientLives,
                        int & clientBombs,
                        bool &speedBonus);

    /**
     * @brief Updates the game information and draws the game board on a screen.
     * This effectively involves reading incoming messages from the Server.
     * @param serverLives Outputs updated lives of the server player.
     * @param serverBombs Outputs updated number of bombs the server player has.
     * @param clientLives Outputs updated lives of the client player.
     * @param clientBombs Outputs updated number of bombs the client player has.
     * @param speedBonus Outputs true if the client player has a speed bonus,
     *  false otherwise.
     */
    void update(int & serverLives,
                int & serverBombs,
                int & clientLives,
                int & clientBombs,
                bool & speedBonus);
private:
    void closeSockets();

    bool mCancelListening;
    std::mutex mCancelListeningLock;

    std::mutex mSocketLock;
    std::mutex mMessageLock;
    std::deque<ServerMessage*> mMessages;
    int mSocket;
    std::chrono::milliseconds ** mTimestamps;
    int mWidth;
    int mHeight;

    int prepareCliSocket(const char * address,
                         const char * port);
    void listen();
};

#endif
