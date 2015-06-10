#include "online_player.h"

#include "game.h"

OnlinePlayer::OnlinePlayer(Game *g,
                           const Pos &pos,
                           int lives,
                           int bombs,
                           Server * s)
    :Enemy(g, pos, lives, bombs),
      mServer(s)
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
            game->movePlayer(*this, Pos(0,1));
        else if ( m == ClientMessage::MOVE_UP )
            game->movePlayer(*this, Pos(0,-1));
        else if ( m == ClientMessage::MOVE_LEFT )
            game->movePlayer(*this, Pos(-1,0));
        else if ( m == ClientMessage::MOVE_RIGHT )
            game->movePlayer(*this, Pos(1,0));
        else if ( m == ClientMessage::PLACE_BOMB )
            game->plantBombAction(*this);
        else if ( m == ClientMessage::DETONATE_BOMBS )
            detonateRemoteBombs();
    }
}
