#ifndef MAP_H
#define MAP_H

#include <vector>

#include "block.h"
#include "player.h"
#include "pos.h"

class Map
{
public:
    class MapLoadException
    {
    public:
        enum Type {READ_FAILURE, INVALID_MAP};
        MapLoadException(Type t):type(t){}
        Type type;
    };

    Map();
    Map(const char * file, Pos & playerPos, Pos & enemyPos); /* throws MapLoadException */
    void load(const char * file, Pos & playerPos, Pos & enemyPos); /* throws MapLoadException */
    ~Map();

    void draw();

    char & at(const Pos & pos);
    char get(const Pos & pos) const;

    int width() const;
    int height() const;
private:
    std::vector<char*> map;

    int mWidth;
    void clearMap();

    static const int maxWidth = 80;
    static const int maxHeight = 23;
};

#endif
