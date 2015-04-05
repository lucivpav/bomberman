#ifndef BLOCK_H
#define BLOCK_H

class Block
{
public:
    class UnknownType
    {
    };

    enum BlockType {BLOCK_EMPTY=0,
                    BLOCK_WALL,
                    BLOCK_PLAYER,
                    BLOCK_DESTRUCTABLE};
    static char typeToSign(BlockType t);
    static BlockType signToType(char c); /* throws UnknownType */
    BlockType type;
private:
    static constexpr const char * signs = " #@%";
};

#endif
