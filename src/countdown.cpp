#include "countdown.h"

#include "game.h"

Countdown::Countdown(int milliseconds)
    :mPrev(Game::getTimestamp()), mPeriod(milliseconds)
{

}


bool Countdown::expired(int milliseconds)
{
    auto cur = Game::getTimestamp();
    if ( (cur - mPrev).count() < mPeriod )
        return false;
    else
    {
        if ( milliseconds != -1 )
            mPeriod = milliseconds;
        mPrev = cur;
        return true;
    }
}
