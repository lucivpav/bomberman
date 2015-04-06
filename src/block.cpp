#include "block.h"

#include <string.h>

char Block::typeToSign(Block::BlockType t)
{
    return signs[t];
}

Block::BlockType Block::signToType(char c)
{
    for ( size_t i = 0 ; i < strlen(signs) ; i++ )
        if ( signs[i] == c )
            return BlockType(i);
    throw UnknownType();
}
