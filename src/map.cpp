#include "map.h"

#include <cassert>
#include <fstream>
#include <ncurses.h>

Map::Map()
{
}

Map::Map(const char *file, Pos & playerPos, Pos &enemyPos, std::set<Pos> & trapsPos)
{
    load(file, playerPos, enemyPos, trapsPos);
}

void Map::load(const char *file, Pos & playerPos, Pos &enemyPos, std::set<Pos> & trapsPos)
{
    const char LF = 10, CR = 13;
    enum Encoding {UNIX, DOS, MAC};
    Encoding encoding;

    clearMap();

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
    mWidth = line.size();
    if ( mWidth < 3 || mWidth > maxWidth )
        throw MapLoadException(MapLoadException::INVALID_MAP);

    map.push_back(new char[mWidth+1]);
    map[0][mWidth] = '\0';
    for ( size_t i = 0 ; i < line.size() ; i++ )
    {
        map[0][i] = line[i];
        if ( !validOuterBlock(line[i]) )
            throw MapLoadException(MapLoadException::INVALID_MAP);
    }

    /* load map */
    int i;
    bool playerFound = false;
    bool enemyFound = false;
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

        if ( (int)line.size() != mWidth
             || !validOuterBlock(line[0])
             || !validOuterBlock(line.back()) )
            throw MapLoadException(MapLoadException::INVALID_MAP);

        map.push_back(new char[mWidth+1]);
        map.back()[mWidth] = '\0';
        for ( size_t j = 0 ; j < line.size() ; j++ )
        {
            char c = line[j];
            if ( !validInnerBlock(c) )
                throw MapLoadException(MapLoadException::INVALID_MAP);
            map[i][j] = c;
            if ( c == Block::typeToSymbol(Block::PLAYER) )
            {
                if ( playerFound ) /* player duplication */
                    throw MapLoadException(MapLoadException::INVALID_MAP);
                playerFound = true;
                playerPos = Pos(j, i);
            }
            else if ( c == Block::typeToSymbol(Block::ENEMY) )
            {
                if ( enemyFound ) /* enemy duplication */
                    throw MapLoadException(MapLoadException::INVALID_MAP);
                enemyFound = true;
                enemyPos = Pos(j, i);
            }
            else if ( c == Block::typeToSymbol(Block::TRAP_OPENED)
                      || c == Block::typeToSymbol(Block::TRAP_CLOSED) )
            {
                map[i][j] = Block::typeToSymbol(Block::EMPTY);
                trapsPos.insert(Pos(j, i));
            }
        }
    }

    if ( i < 3 || !playerFound || !enemyFound)
        throw MapLoadException(MapLoadException::INVALID_MAP);

    /* check last line */
    for ( int i = 1 ; i < mWidth-1 ; i++ )
    {
        if ( !validOuterBlock(map.back()[i]) )
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
        mvprintw(i, 0, map[i]); //null terminator necessary
}

char & Map::at(const Pos &pos)
{
    assert ( pos.y >= 0 && pos.y < height() );
    assert ( pos.x >= 0 && pos.x < width() );
    return map[pos.y][pos.x];
}

char Map::get(const Pos &pos) const
{
    assert ( pos.y >= 0 && pos.y < height() );
    assert ( pos.x >= 0 && pos.x < width() );
    return map[pos.y][pos.x];
}

int Map::width() const
{
    return mWidth;
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

bool Map::validOuterBlock(char symbol) const
{
    return symbol == Block::typeToSymbol(Block::EMPTY)
            || symbol == Block::typeToSymbol(Block::WALL)
            || symbol == Block::typeToSymbol(Block::TRAP_CLOSED)
            || symbol == Block::typeToSymbol(Block::TRAP_OPENED)
            || symbol == Block::typeToSymbol(Block::DESTRUCTABLE);
}

bool Map::validInnerBlock(char symbol) const
{
    return symbol == Block::typeToSymbol(Block::EMPTY)
            || symbol == Block::typeToSymbol(Block::WALL)
            || symbol == Block::typeToSymbol(Block::PLAYER)
            || symbol == Block::typeToSymbol(Block::ENEMY)
            || symbol == Block::typeToSymbol(Block::TRAP_CLOSED)
            || symbol == Block::typeToSymbol(Block::TRAP_OPENED)
            || symbol == Block::typeToSymbol(Block::DESTRUCTABLE);
}
