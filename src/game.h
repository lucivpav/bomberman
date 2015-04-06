#ifndef GAME_H
#define GAME_H

#include "map.h"

class Game
{
public:
    Game();
    ~Game();
private:
    Map map;

    void loop();
};

#endif
