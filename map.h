#ifndef MAP_H
#define MAP_H

#include <vector>

#include "block.h"

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

    Map(const char * file); /* throws MapLoadException */
    void load(const char * file); /* throws MapLoadException */
    ~Map();

    void draw();
private:
    std::vector<Block*> map;
    int width;
    int height() const;
    void clearMap();

    static const int maxWidth = 80;
    static const int maxHeight = 24;
};

#endif
