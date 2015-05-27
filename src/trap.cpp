#include "trap.h"

#include <ctime>
#include <cstdlib>

#include "map.h"

Map * Trap::mMap;

Trap::Trap(Map &map, const Pos &pos)
    :mPos(pos), mCountdown(0)
{
    mMap = &map;
    if ( rand() % 5 )
    {
        mOpened = false;
        mCountdown.expired(10000);
    }
    else
    {
        mOpened = true;
        mCountdown.expired(5000);
    }
}

Pos Trap::getPos() const
{
    return mPos;
}

void Trap::update()
{
    if ( !mCountdown.expired(mOpened ? 10000 : 5000) )
        return;
    if ( !mOpened && rand() % 3 )
        return;
    mOpened = !mOpened;
}

bool Trap::isOpen() const
{
    return mOpened;
}
