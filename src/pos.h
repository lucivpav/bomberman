#ifndef POS_H
#define POS_H

#include <cmath>

struct Pos
{
    Pos();
    Pos(int x, int y);
    int x;
    int y;

    Pos operator+(const Pos & pos) const;
    Pos operator-(const Pos & pos) const;
    Pos & operator+=(const Pos & pos);
    Pos & operator-=(const Pos & pos);
    bool operator==(const Pos & pos) const;
    bool operator!=(const Pos & pos) const;
    bool operator<(const Pos & pos) const;

    static int manhattanDistance(const Pos & a, const Pos & b);
    static int airDistance(const Pos & a, const Pos & b);
};

#endif
