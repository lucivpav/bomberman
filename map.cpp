#include "map.h"

#include <fstream>
#include <ncurses.h>

Map::Map(const char *file)
{
    load(file);
}

void Map::load(const char *file)
{
    const char LF = 10, CR = 13;
    enum Encoding {UNIX, DOS, MAC};
    Encoding encoding;

    clearMap();
    map.clear();

    if ( !file )
        throw MapLoadException(MapLoadException::READ_FAILURE);

    std::ifstream f(file, std::ifstream::in | std::ifstream::binary );
    std::string line;
    if ( !f )
        throw MapLoadException(MapLoadException::READ_FAILURE);

    /* figure out line terminators */
    while (1)
    {
        char c;
        if ( !f.get(c) )
            throw MapLoadException(MapLoadException::READ_FAILURE);

        if ( c == LF )
        {
            encoding = Encoding::UNIX;
            break;
        }
        else if ( c == CR )
        {
            if ( !f.get(c) )
                throw MapLoadException(MapLoadException::READ_FAILURE);
            if ( c == LF )
            {
                encoding = Encoding::DOS;
                break;
            }
            if ( !f.unget() )
                throw MapLoadException(MapLoadException::READ_FAILURE);
            encoding = Encoding::MAC;
            break;
        }

        line.push_back(c);
    }

    /* check first line */
    width = line.size();
    if ( width < 3 || width > maxWidth )
        throw MapLoadException(MapLoadException::INVALID_MAP);

    map.push_back(new Block[width]);
    for ( int i = 0 ; i < line.size() ; i++ )
    {
        map[0][i].type = Block::BLOCK_WALL;
        if ( line[i] != Block::typeToSign(Block::BLOCK_WALL) )
            throw MapLoadException(MapLoadException::INVALID_MAP);
    }

    /* load map */
    int i;
    bool playerFound = false;
    for ( i = 1 ; ; i++ )
    {
        getline(f, line, encoding == Encoding::UNIX ? LF : CR);
        if ( !f )
            break;

        if ( i >= maxHeight )
            throw MapLoadException(MapLoadException::INVALID_MAP);

        if ( encoding == Encoding::DOS )
        {
            char c;
            if ( f.get(c) && c != LF )
                throw MapLoadException(MapLoadException::READ_FAILURE);
        }

        if ( line.size() != width ||
             line[0] != Block::typeToSign(Block::BLOCK_WALL) ||
             line.back() != Block::typeToSign(Block::BLOCK_WALL) )
            throw MapLoadException(MapLoadException::INVALID_MAP);

        map.push_back(new Block[width]);
        for ( int j = 0 ; j < line.size() ; j++ )
        {
            char c = line[j];
            try {
                Block::BlockType type = Block::signToType(c);
                map[i][j].type = type;
                if ( type == Block::BLOCK_PLAYER )
                {
                    if ( playerFound ) /* player duplication */
                        throw MapLoadException(MapLoadException::INVALID_MAP);
                    playerFound = true;
                }
            } catch ( const Block::UnknownType & ) {
                throw MapLoadException(MapLoadException::INVALID_MAP);
            }
        }
    }

    if ( i < 3 || !playerFound )
        throw MapLoadException(MapLoadException::INVALID_MAP);

    /* check last line */
    for ( int i = 1 ; i < width-1 ; i++ )
    {
        if ( map.back()[i].type != Block::BLOCK_WALL )
            throw MapLoadException(MapLoadException::INVALID_MAP);
    }
}

Map::~Map()
{
    for ( auto line : map )
        delete [] line;
}

void Map::draw()
{
    for ( int i = 0 ; i < height() ; i++ )
        for ( int j = 0 ; j < width ; j++ )
            mvaddch(i, j, Block::typeToSign(map[i][j].type));
}

int Map::height() const
{
    return map.size();
}

void Map::clearMap()
{
    for ( auto line : map )
        delete [] line;
    map.clear();
}
