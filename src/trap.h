#ifndef TRAP_H
#define TRAP_H

#include "pos.h"
#include "countdown.h"

class Map;

class Trap
{
public:
    Trap(Map & map, const Pos & pos);
    Pos getPos() const;
    void update();
    bool isOpen() const;
private:
    static Map * mMap;

    Pos mPos;
    bool mOpened;
    Countdown mCountdown; /* change mode countdown */
};

#endif
