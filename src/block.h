#ifndef BLOCK_H
#define BLOCK_H

class Block
{
public:
    enum BlockType {BLOCK_EMPTY=0,
                    BLOCK_WALL,
                    BLOCK_PLAYER,
                    BLOCK_DESTRUCTABLE};
    static char typeToSign(BlockType t);
    static bool validSymbol(char c);
private:
    static constexpr const char * signs = " Wcz";
};

#endif
