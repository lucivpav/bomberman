#include "pos.h"

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
    return toReturn.x + toReturn.y;
}

int Pos::airDistance(const Pos &a, const Pos &b)
{
    Pos toReturn = a - b;
    return sqrt(toReturn.x * toReturn.x + toReturn.y * toReturn.y);
}
