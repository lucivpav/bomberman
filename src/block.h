#ifndef BLOCK_H
#define BLOCK_H

/**
 * @brief The Block class represents a single object on the map.
 */
class Block
{
public:
    /**
     * @brief The Type enum represents a type of the Block.
     */
    enum Type { EMPTY = 0,
                WALL,
                NICE_WALL,
                PLAYER,
                ENEMY,
                GHOST,
                TRAP_CLOSED,
                TRAP_OPENED,
                DESTRUCTABLE,
                TIMED_BOMB,
                FLAME,
                BONUS_BOMB = 11,
                BONUS_SPEED,
                BONUS_REMOTE,
                BONUS_RADIUS,
                REMOTE_BOMB };

    /**
     * @brief Converts Type to a symbol -
     * a char representing an object on the screen.
     * @param type Type to be converted.
     * @return A symbol associated with the given type.
     */
    static char typeToSymbol(Type type);

    /**
     * @brief Converts symbol, a graphical representation of a Block,
     * to Type.
     * @param symbol A symbol to be converted.
     * @return A Type associated with the given symbol.
     */
    static Type symbolToType(char symbol);

    /**
     * @brief Checks whether the given symbol is a solid block,
     * i.e. players are not allowed to walk through them.
     * @param symbol The symbol to be checked.
     * @return true if the symbol is solid, false otherwise.
     */
    static bool isSolid(char symbol);
private:
    /* todo: ensure items such as flames cannot be loaded from a file */
    static constexpr const char * signs = " WOcegTt.b#*^&@x";
    static constexpr const char * solid = "0111100011000001";
};

#endif
