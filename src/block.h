#ifndef BLOCK_H
#define BLOCK_H

class Block
{
public:
    enum Type { EMPTY = 0,
                WALL,
                PLAYER,
                ENEMY,
                GHOST,
                DESTRUCTABLE,
                TIMED_BOMB,
                FLAME,
                BONUS_BOMB = 8,
                BONUS_SPEED,
                BONUS_REMOTE,
                BONUS_RADIUS,
                REMOTE_BOMB };
    static char typeToSymbol(Type t);
    static Type symbolToType(char symbol);
    static bool validSymbol(char symbol);
    static bool isSolid(char symbol);
private:
    /* todo: ensure items like flames cannot be loaded from a file */
    static constexpr const char * signs = " Wcegzb#*^&@x";
    static constexpr const char * solid = "0111011000001";
};

#endif
