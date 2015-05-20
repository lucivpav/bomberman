#include "countdown.h"

Countdown::Countdown(int milliseconds)
    :mPrev(getTimestamp()), mPeriod(milliseconds)
{

}


bool Countdown::expired(int milliseconds)
{
    auto cur = getTimestamp();
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

std::chrono::milliseconds Countdown::getTimestamp() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now().time_since_epoch());
}
