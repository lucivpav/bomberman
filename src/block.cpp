#include "block.h"

#include <string.h>
#include <assert.h>

char Block::typeToSymbol(Block::Type t)
{
    return signs[t];
}

bool Block::validSymbol(char symbol)
{
    Type t = symbolToType(symbol);
    return (int)t == -1 ? false : true;
}

bool Block::isSolid(char symbol)
{
    Type t = symbolToType(symbol);
    assert ( (int)t != -1 );
    return solid[t]-48;
}

Block::Type Block::symbolToType(char symbol)
{
    for ( size_t i = 0 ; i < strlen(signs) ; i++ )
        if ( signs[i] == symbol )
            return (Type)i;
    return (Type)-1;
}
