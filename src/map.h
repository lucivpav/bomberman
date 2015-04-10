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
    Map(const char * file, Pos & playerPos); /* throws MapLoadException */
    void load(const char * file, Pos & playerPos); /* throws MapLoadException */
    ~Map();

    void draw();

    char & at(const Pos & pos);
private:
    std::vector<char*> map;

    int width;
    int height() const;
    void clearMap();

    static const int maxWidth = 80;
    static const int maxHeight = 24;
};

#endif
