#ifndef BLOCK_H
#define BLOCK_H

class Block
{
public:
    enum Type {EMPTY = 0,
               WALL,
               PLAYER,
               DESTRUCTABLE,
               BOMB,
               FLAME};
    static char typeToSymbol(Type t);
    static bool validSymbol(char symbol);
    static bool isSolid(char symbol);
private:
    static Type symbolToType(char symbol);
    static constexpr const char * signs = " Wczb#";
    static constexpr const char * solid = "011110";
};

#endif
