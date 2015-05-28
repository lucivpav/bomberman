#include "pos.h"

#include <cassert>

Pos::Pos()
{

}

Pos::Pos(int x, int y)
    :x(x), y(y)
{
}


Pos Pos::operator+(const Pos &pos) const
{
    return Pos(x+pos.x, y+pos.y);
}

Pos Pos::operator-(const Pos &pos) const
{
    return Pos(x-pos.x, y-pos.y);
}

Pos &Pos::operator+=(const Pos &pos)
{
   x += pos.x;
   y += pos.y;
   return *this;
}

Pos &Pos::operator-=(const Pos &pos)
{
   x -= pos.x;
   y -= pos.y;
   return *this;
}

bool Pos::operator==(const Pos &pos) const
{
   return ( x == pos.x ) && ( y == pos.y );
}

bool Pos::operator!=(const Pos &pos) const
{
    return !(*this == pos);
}

int Pos::manhattanDistance(const Pos &a, const Pos &b)
{
    Pos toReturn = a - b;
    if ( toReturn.x < 0 )
        toReturn.x *= (-1);
    if ( toReturn.y < 0 )
        toReturn.y *= (-1);
    assert ( toReturn.x >= 0 );
    assert ( toReturn.y >= 0 );
    return toReturn.x + toReturn.y;
}

bool Pos::operator<(const Pos &pos) const
{
    if ( x < pos.x )
        return true;
    else if ( x > pos.x )
        return false;
    if ( y < pos.y )
        return true;
    else if ( y > pos.y )
        return false;
    return false;
}
