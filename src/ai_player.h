#ifndef AI_PLAYER_H
#define AI_PLAYER_H

#include "enemy.h"
#include "countdown.h"

#include <map>
#include <vector>

class AIPlayer : public Enemy
{
public:
    AIPlayer(Game * g, Player * p, const Pos & pos, int lives, int bombs);
    virtual ~AIPlayer();

    virtual void makeDecision();

#ifdef DEBUG
    mutable std::vector<Pos> mPath;
    mutable std::vector<Pos> mCandidatePath;
#endif
private:
    enum Action {IDLE, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN,
                 PLANT_BOMB, DETONATE_BOMBS};

    Action getBestAction();
    void updateBestAction(const Action & action,
                          Action & bestAction,
                          int & bestUtility);

    int idleUtility();
    int moveUtility(const Pos & offset);
    int plantBombUtility();
    int detonateBombsUtility() const;

    void moveAction(const Pos & offset);

    bool canFlee(const Bomb &threat) const;
    int canFleeDirection(const Bomb & threat, Pos offset) const;
    bool bombThreat(Pos location,
                    const Bomb ** bomb = 0,
                    const Bomb * ignore = 0) const;
    const Bomb *bombThreatDirection(const Pos &location,
                                    const Pos &offset,
                                    const Bomb * ignore = 0) const;
    int wallsToBeDestroyed() const;
    bool wallToBeDestroyedDirection(const Pos & offset) const;
    bool bonusOpportunity(const Pos & offset) const;

    int distanceToEnemy(Pos from, std::vector<Pos> * hint = 0) const;
    void reconstructPath(const Pos & from,
                         const Pos & to,
                         const std::map<Pos, Pos> & prev,
                         std::vector<Pos> & path) const;

    Player * enemy;
    Countdown mIdleCountdown;
    Countdown mMoveCountdown;
    bool mIdle;
    std::vector<Pos> mPathfinderHint;
};

#endif
