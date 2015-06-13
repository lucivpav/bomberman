#include "online_player.h"

#include "game.h"

OnlinePlayer::OnlinePlayer(Game *game,
                           const Pos &pos,
                           int lives,
                           int bombs,
                           Server * server)
    :Enemy(game, pos, lives, bombs),
      mServer(server)
{

}

OnlinePlayer::~OnlinePlayer()
{

}

void OnlinePlayer::makeDecision()
{
    ClientMessage m;
    if ( mServer->getMessage(m) )
    {
        if ( m == ClientMessage::MOVE_DOWN )
            mGame->movePlayer(*this, Pos(0,1));
        else if ( m == ClientMessage::MOVE_UP )
            mGame->movePlayer(*this, Pos(0,-1));
        else if ( m == ClientMessage::MOVE_LEFT )
            mGame->movePlayer(*this, Pos(-1,0));
        else if ( m == ClientMessage::MOVE_RIGHT )
            mGame->movePlayer(*this, Pos(1,0));
        else if ( m == ClientMessage::PLACE_BOMB )
            mGame->plantBombAction(*this);
        else if ( m == ClientMessage::DETONATE_BOMBS )
            detonateRemoteBombs();
    }
}
