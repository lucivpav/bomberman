#include "ghost.h"

#include "game.h"
#include <cassert>

Ghost::Ghost(Game *game, const Pos &pos)
    :mGame(game), mPos(pos), mDirection(Pos(1, 0)),
      mCountdown(4)
{
    if ( rand() % 4 )
        changeDirection();
}

void Ghost::setPos(const Pos & pos)
{
    mPos = pos;
}

const Pos & Ghost::getPos() const
{
    return mPos;
}

bool Ghost::makeDecision()
{
    if ( !mCountdown && rand() % 3 )
    {
        if ( changeDirection() )
            if ( !mGame->moveGhost(*this, mDirection) )
                return false;
        mCountdown = 4;
        return true;
    }
    else
    {
        Pos newPos = mPos + mDirection;
        bool canMove = true;
        if ( !mGame->canMoveGhost(newPos) )
            if ( !changeDirection() )
                canMove = false;
        if  ( canMove )
            if ( !mGame->moveGhost(*this, mDirection) )
                return false;
        if ( mCountdown )
            --mCountdown;
        return true;
    }
}

bool Ghost::changeDirection()
{
    Pos newDirection[3];
    newDirection[0] = Pos(-mDirection.x, -mDirection.y);
    newDirection[1] = Pos(mDirection.y, mDirection.x);
    newDirection[2] = Pos(mDirection.y * -1, mDirection.x * -1);

    int choice = rand() % 3;

    for ( int i = 0 ; i < 3 ; i++ )
    {
        if ( !mGame->canMoveGhost(mPos + newDirection[choice]) )
            choice = (choice+1) % 3;
        else
        {
            mDirection = newDirection[choice];
            return true;
        }
    }
    return false;
}
