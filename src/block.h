#ifndef BLOCK_H
#define BLOCK_H

class Block
{
public:
    // editable == may show up in the map file
    enum Type { EMPTY = 0, // editable
                WALL, // editable
                NICE_WALL, // editable
                PLAYER, // editable
                ENEMY, // editable
                GHOST,
                TRAP_CLOSED, // editable
                TRAP_OPENED, // editable
                DESTRUCTABLE, // editable
                TIMED_BOMB,
                FLAME,
                BONUS_BOMB = 11,
                BONUS_SPEED,
                BONUS_REMOTE,
                BONUS_RADIUS,
                REMOTE_BOMB };
    static char typeToSymbol(Type t);
    static Type symbolToType(char symbol);
    static bool isSolid(char symbol);
private:
    /* todo: ensure items like flames cannot be loaded from a file */
    static constexpr const char * signs = " WOcegTtzb#*^&@x";
    static constexpr const char * solid = "0111100011000001";
};

#endif
