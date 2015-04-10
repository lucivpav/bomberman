#include "map.h"

#include <fstream>
#include <ncurses.h>

Map::Map()
{
}

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

    map.push_back(new char[width+1]);
    map[0][width] = '\0';
    for ( size_t i = 0 ; i < line.size() ; i++ )
    {
        map[0][i] = Block::typeToSign(Block::BLOCK_WALL);
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

        if ( (int)line.size() != width ||
             line[0] != Block::typeToSign(Block::BLOCK_WALL) ||
             line.back() != Block::typeToSign(Block::BLOCK_WALL) )
            throw MapLoadException(MapLoadException::INVALID_MAP);

        map.push_back(new char[width+1]);
        map.back()[width] = '\0';
        for ( size_t j = 0 ; j < line.size() ; j++ )
        {
            char c = line[j];
            if ( !Block::validSymbol(c) )
                throw MapLoadException(MapLoadException::INVALID_MAP);
            map[i][j] = c;
            if ( c == Block::typeToSign(Block::BLOCK_PLAYER) )
            {
                if ( playerFound ) /* player duplication */
                    throw MapLoadException(MapLoadException::INVALID_MAP);
                playerFound = true;
                player.setPos(Pos(i,j));
            }
        }
    }

    if ( i < 3 || !playerFound )
        throw MapLoadException(MapLoadException::INVALID_MAP);

    /* check last line */
    for ( int i = 1 ; i < width-1 ; i++ )
    {
        if ( map.back()[i] != Block::typeToSign(Block::BLOCK_WALL) )
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

void Map::keyEvent(int key)
{
    if ( key == KEY_DOWN || key == 's' )
    {
        Pos p = player.getPos();
        p.x++;
        if ( p.x > height()-2 )
            return;
        map[p.x-1][p.y] = Block::typeToSign(Block::BlockType::BLOCK_EMPTY);
        map[p.x][p.y] = Block::typeToSign(Block::BlockType::BLOCK_PLAYER);
        player.setPos(p);
    }
    else if ( key == KEY_UP || key == 'w' )
    {
        Pos p = player.getPos();
        p.x--;
        if ( p.x < 1 )
            return;
        map[p.x+1][p.y] = Block::typeToSign(Block::BlockType::BLOCK_EMPTY);
        map[p.x][p.y] = Block::typeToSign(Block::BlockType::BLOCK_PLAYER);
        player.setPos(p);
    }
    else if ( key == KEY_LEFT || key == 'a' )
    {
        Pos p = player.getPos();
        p.y--;
        if ( p.y < 1 )
            return;
        map[p.x][p.y+1] = Block::typeToSign(Block::BlockType::BLOCK_EMPTY);
        map[p.x][p.y] = Block::typeToSign(Block::BlockType::BLOCK_PLAYER);
        player.setPos(p);

    }
    else if ( key == KEY_RIGHT || key == 'd' )
    {
        Pos p = player.getPos();
        p.y++;
        if ( p.y > width-2 )
            return;
        map[p.x][p.y-1] = Block::typeToSign(Block::BlockType::BLOCK_EMPTY);
        map[p.x][p.y] = Block::typeToSign(Block::BlockType::BLOCK_PLAYER);
        player.setPos(p);
    }
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
