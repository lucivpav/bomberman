#include "ghost.h"

#include "game.h"
#include <cassert>

Ghost::Ghost(Game *g, const Pos &pos, char ground)
    :mGame(g), mPos(pos), mDirection(Pos(1, 0)),
      mCountdown(4),mGround(ground)
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

void Ghost::setGround(char ground)
{
    mGround = ground;
}

char Ghost::getGround() const
{
    return mGround;
}

void Ghost::makeDecision()
{
    if ( !mCountdown && rand() % 3 )
    {
        changeDirection();
        mGame->moveGhost(*this, mDirection);
        mCountdown = 4;
    }
    else
    {
        Pos newPos = mPos + mDirection;
        if ( !mGame->canMoveGhost(newPos) )
            changeDirection();
        mGame->moveGhost(*this, mDirection);
        if ( mCountdown )
            --mCountdown;
    }
}

void Ghost::changeDirection()
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
            return;
        }
    }
    assert ( false );
}
