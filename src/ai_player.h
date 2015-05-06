#ifndef AI_PLAYER_H
#define AI_PLAYER_H

#include "player.h"

class AIPlayer : public Player
{
public:
    AIPlayer(Game * g, Player * p, const Pos & pos, int lives, int bombs);
    virtual ~AIPlayer();
    virtual char getSymbol() const;
    void makeDecision();
private:
    enum Action {IDLE, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN,
                 PLANT_BOMB, DETONATE_BOMBS};

    Action getBestAction();
    void updateBestAction(const Action & action,
                          Action & bestAction,
                          int & bestUtility);
    int manhattanDistance(const Pos & a, const Pos & b) const;
    int airDistance(const Pos & a, const Pos & b) const;

    int idleUtility() const;
    int moveUtility(const Pos & offset) const;

    int plantBombUtility() const;
    bool canFlee(const Bomb &threat) const;
    bool canFleeDirection(const Bomb & threat, Pos offset) const;
    bool bombThreat(Pos location, const Bomb **bomb = 0) const;
    const Bomb *bombThreatDirection(const Pos &location, const Pos &offset) const;
    int wallsToBeDestroyed() const;
    bool wallToBeDestroyedDirection(const Pos & offset) const;

    int detonateBombsUtility() const;

    void moveAction(const Pos & offset);
    void plantBombAction();

    Player * enemy;
};

#endif
