#ifndef BLOCK_H
#define BLOCK_H

class Block
{
public:
    // editable == may show up in the map file
    enum Type { EMPTY = 0, // editable
                WALL, // editable
                PLAYER, // editable
                ENEMY, // editable
                GHOST,
                TRAP_CLOSED, // editable
                TRAP_OPENED, // editable
                DESTRUCTABLE, // editable
                TIMED_BOMB,
                FLAME,
                BONUS_BOMB = 10,
                BONUS_SPEED,
                BONUS_REMOTE,
                BONUS_RADIUS,
                REMOTE_BOMB };
    static char typeToSymbol(Type t);
    static Type symbolToType(char symbol);
    static bool isSolid(char symbol);
private:
    /* todo: ensure items like flames cannot be loaded from a file */
    static constexpr const char * signs = " WcegTtzb#*^&@x";
    static constexpr const char * solid = "011100011000001";
};

#endif
