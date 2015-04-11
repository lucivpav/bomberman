#ifndef BLOCK_H
#define BLOCK_H

class Block
{
public:
    enum Type { EMPTY = 0,
                WALL,
                PLAYER,
                DESTRUCTABLE,
                BOMB,
                FLAME,
                BONUS_BOMB = 6,
                BONUS_SPEED,
                BONUS_REMOTE,
                BONUS_RADIUS };
    static char typeToSymbol(Type t);
    static Type symbolToType(char symbol);
    static bool validSymbol(char symbol);
    static bool isSolid(char symbol);
private:
    static constexpr const char * signs = " Wczb#*^&@";
    static constexpr const char * solid = "0111100000";
};

#endif
