#include "clientgame.h"
#include "ui.h"
#include "countdown.h"

#include <ncurses.h>

ClientGame::ClientGame(const char *address, const char *port)
    :mAddress(address),
      mPort(port),
      mExpired(false)
{
    if ( !mClient.connect(address, port) )
    {
        UI::Notification("Error: Cannot connect to the server");
        return;
    }


    if ( !mClient.initOnlineGame(mEnemyLives, mEnemyBombs,
                                 mLives, mBombs, mSpeedBonus) )
    {
        UI::Notification("Error: Server not responding.");
        return;
    }

    loop();
}

void ClientGame::loop()
{
    while ( 1 )
    {
        if ( mExpired )
            break;

        if ( !mClient.isConnected() && !mClient.pendingMessages() )
        {
            networkErrorAction();
            break;
        }

        int c = getch();

        if ( c != ERR )
            keyEvent(c);

        mClient.update(mEnemyLives, mEnemyBombs,
                       mLives, mBombs, mSpeedBonus);

        drawStatus();
        refresh();

        if ( mLives == 0 )
            loseAction();
        else if ( mEnemyLives == 0 )
            winAction();
    }
}

void ClientGame::keyEvent(int key)
{
    static bool moveButtonPressed = false;
    bool moveButtonHold = false;

    // not holding on first iteration
    static Countdown buttonCountdown(0);
    moveButtonHold = !buttonCountdown.expired(100);

    if ( key == 'q' )
    {
        mExpired = true;
    }
    else if ( key == KEY_DOWN || key == 's' )
    {
        if ( !moveButtonHold || moveButtonPressed || mSpeedBonus )
            mClient.sendMessage(ClientMessage::MOVE_DOWN);
        moveButtonPressed = !moveButtonPressed;
    }
    else if ( key == KEY_UP || key == 'w' )
    {
        if ( !moveButtonHold || moveButtonPressed || mSpeedBonus )
            mClient.sendMessage(ClientMessage::MOVE_UP);
        moveButtonPressed = !moveButtonPressed;
    }
    else if ( key == KEY_LEFT || key == 'a' )
    {
        if ( !moveButtonHold || moveButtonPressed || mSpeedBonus )
            mClient.sendMessage(ClientMessage::MOVE_LEFT);
        moveButtonPressed = !moveButtonPressed;
    }
    else if ( key == KEY_RIGHT || key == 'd' )
    {
        if ( !moveButtonHold || moveButtonPressed || mSpeedBonus )
            mClient.sendMessage(ClientMessage::MOVE_RIGHT);
        moveButtonPressed = !moveButtonPressed;
    }
    else if ( key == 'b' )
    {
        mClient.sendMessage(ClientMessage::PLACE_BOMB);
    }
    else if ( key == ' ' )
    {
        mClient.sendMessage(ClientMessage::DETONATE_BOMBS);
    }
}

void ClientGame::drawStatus() const
{
    int width, height;
    getmaxyx(stdscr, height, width);

    std::string status =
            "LIVES: " +
            std::to_string(mLives) +
            " BOMBS: " +
            std::to_string(mBombs);

    std::string enemyStatus =
            "LIVES: " +
            std::to_string(mEnemyLives) +
            " BOMBS: " +
            std::to_string(mEnemyBombs);
    mvprintw(height-1, 0, status.c_str());
    mvprintw(height-1, width-enemyStatus.size()-1, enemyStatus.c_str());
}

void ClientGame::winAction()
{
    UI::Notification("You won!");
    mExpired = true;
}

void ClientGame::loseAction()
{
    UI::Notification("You lost!");
    mExpired = true;
}

void ClientGame::networkErrorAction()
{
    UI::Notification("Network error");
}
