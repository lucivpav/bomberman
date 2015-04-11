#ifndef POS_H
#define POS_H

struct Pos
{
    Pos();
    Pos(int x, int y);
    int x;
    int y;

    Pos operator+(const Pos & pos) const;
    Pos & operator+=(const Pos & pos);
    bool operator==(const Pos & pos) const;
    bool operator!=(const Pos & pos) const;
};

#endif
