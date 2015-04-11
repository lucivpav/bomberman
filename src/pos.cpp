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

Pos &Pos::operator+=(const Pos &pos)
{
   x += pos.x;
   y += pos.y;
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
