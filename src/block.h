#ifndef BLOCK_H
#define BLOCK_H

class Block
{
public:
    enum Type { EMPTY = 0,
                WALL,
                PLAYER,
                ENEMY,
                DESTRUCTABLE,
                TIMED_BOMB,
                FLAME,
                BONUS_BOMB = 7,
                BONUS_SPEED,
                BONUS_REMOTE,
                BONUS_RADIUS,
                REMOTE_BOMB};
    static char typeToSymbol(Type t);
    static Type symbolToType(char symbol);
    static bool validSymbol(char symbol);
    static bool isSolid(char symbol);
private:
    /* todo: ensure items like flames cannot be loaded from a file */
    static constexpr const char * signs = " Wcezb#*^&@x";
    static constexpr const char * solid = "011111000001";
};

#endif
