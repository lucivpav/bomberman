#include "ai_player.h"

#include "game.h"
#include "block.h"

#include <cmath>
#include <cassert>

AIPlayer::AIPlayer(Game *g, Player *p, const Pos &pos, int lives, int bombs)
    :Player(g, pos, lives, bombs),
      enemy(p)
{
}

AIPlayer::~AIPlayer()
{
}

char AIPlayer::getSymbol() const
{
    return Block::typeToSymbol(Block::ENEMY);
}

void AIPlayer::makeDecision()
{
    Action action = getBestAction();

    if ( action == Action::IDLE )
        return;
    else if ( action == MOVE_LEFT )
        moveAction(Pos(-1, 0));
    else if ( action == MOVE_RIGHT )
        moveAction(Pos(1, 0));
    else if ( action == MOVE_UP )
        moveAction(Pos(0, -1));
    else if ( action == MOVE_DOWN )
        moveAction(Pos(0, 1));
    else if ( action == PLANT_BOMB )
        plantBombAction();
    else if ( action == DETONATE_BOMBS )
        detonateRemoteBombs();
}

AIPlayer::Action AIPlayer::getBestAction()
{
    Action action;
    int utility = 0;

    const Bomb * threat;
    if ( bombThreat(getPos(), &threat) && threat )
    {
        int minStepsRequired = 666;
        int steps;

        steps = canFleeDirection(*threat, Pos(1, 0));
        if ( steps )
        {
            action = MOVE_RIGHT;
            minStepsRequired = steps;
        }

        steps = canFleeDirection(*threat, Pos(-1, 0));
        if ( steps && steps < minStepsRequired )
        {
            action = MOVE_LEFT;
            minStepsRequired = steps;
        }

        steps = canFleeDirection(*threat, Pos(0, 1));
        if ( steps && steps < minStepsRequired )
        {
            action = MOVE_DOWN;
            minStepsRequired = steps;
        }

        steps = canFleeDirection(*threat, Pos(0, -1));
        if ( steps && steps < minStepsRequired )
        {
            action = MOVE_UP;
            minStepsRequired = steps;
        }

        if ( steps == 666 )
            updateBestAction(DETONATE_BOMBS, action, utility);
        return action;
    }
    updateBestAction(Action::IDLE, action, utility);
    updateBestAction(MOVE_LEFT, action, utility);
    updateBestAction(MOVE_RIGHT, action, utility);
    updateBestAction(MOVE_UP, action, utility);
    updateBestAction(MOVE_DOWN, action, utility);
    updateBestAction(PLANT_BOMB, action, utility);
    return action;
}

void AIPlayer::updateBestAction(const Action & action,
                                Action & bestAction,
                                int & bestUtility)
{
    int utility;
    if ( action == Action::IDLE )
        utility = idleUtility();
    else if ( action == MOVE_LEFT )
        utility = moveUtility(Pos(-1, 0));
    else if ( action == MOVE_RIGHT )
        utility = moveUtility(Pos(1, 0));
    else if ( action == MOVE_UP )
        utility = moveUtility(Pos(0, -1));
    else if ( action == MOVE_DOWN )
        utility = moveUtility(Pos(0, 1));
    else if ( action == PLANT_BOMB )
        utility = plantBombUtility();
    else if ( action == DETONATE_BOMBS )
        utility = detonateBombsUtility();
    else
        assert ( false );

    if ( utility > bestUtility )
    {
        bestUtility = utility;
        bestAction = action;
    }
}

int AIPlayer::manhattanDistance(const Pos &a, const Pos &b) const
{
    Pos toReturn = a - b;
    return toReturn.x + toReturn.y;
}

int AIPlayer::airDistance(const Pos &a, const Pos &b) const
{
    Pos toReturn = a - b;
    return sqrt(toReturn.x * toReturn.x + toReturn.y * toReturn.y);
}

/* check for curPos safety before calling this */
int AIPlayer::idleUtility() const
{
    return 200 - manhattanDistance(getPos(), enemy->getPos());
}

int AIPlayer::moveUtility(const Pos &offset) const
{
    Pos pos = getPos() + offset;
    if ( !game->canMovePlayer(pos) || bombThreat(pos) )
        return 0;
    if ( bonusOpportunity(offset) )
        return 400;
    return 200 - manhattanDistance(getPos() + offset, enemy->getPos());
}

int AIPlayer::plantBombUtility() const
{
    if ( !game->canPlantBomb(*this)
         || !canFlee(Bomb(0, getPos(), getBombRadius())) )
        return 0;
    int dist = airDistance(getPos(), enemy->getPos());
    if ( dist < getBombRadius() )
        return 400 - dist;
    return wallsToBeDestroyed() * 160;
}

bool AIPlayer::canFlee(const Bomb & threat) const
{
    return canFleeDirection(threat, Pos(-1, 0))
           || canFleeDirection(threat, Pos(1, 0))
           || canFleeDirection(threat, Pos(0, 1))
           || canFleeDirection(threat, Pos(0, -1));
}

int AIPlayer::canFleeDirection(const Bomb &threat,
                                Pos offset) const
{
    Pos playerPos = getPos();
    for ( int steps = 1 ; ; steps++ )
    {
        playerPos += offset;
        Pos side = Pos(offset.y, offset.x) + playerPos;
        Pos side2 = Pos(offset.y * -1, offset.x * -1) + playerPos;

        if ( !game->canMovePlayer(playerPos) )
            return 0;
        const Bomb * threat2;
        if ( bombThreat(playerPos, &threat2, &threat) )
            return 0;
        {
            if ( playerPos.x == threat.getPos().x
                 || playerPos.y == threat.getPos().y )
            {
                if ( manhattanDistance(playerPos, threat.getPos()) > getBombRadius() )
                    return steps;
            }
            else
                return steps;
        }
        if ( (game->canMovePlayer(side) && !bombThreat(side))
             || (game->canMovePlayer(side2) && !bombThreat(side2)) )
            return steps;
    }
}

bool AIPlayer::bombThreat(Pos location, const Bomb ** bomb, const Bomb * ignore) const
{
    Block::Type curBlock = Block::symbolToType(game->getMap().get(location));
    if ( curBlock == Block::TIMED_BOMB || curBlock == Block::REMOTE_BOMB )
    {
        if ( bomb )
            *bomb = game->getBomb(getPos());
        if ( !ignore || (ignore && *bomb != ignore) )
            return true;
    }
    else if ( curBlock == Block::FLAME )
    {
        if ( bomb )
            *bomb = 0;
        return true;
    }
    const Bomb * b;
    if ( (b = bombThreatDirection(location, Pos(1, 0), ignore)) )
    {
        if ( bomb )
            *bomb = b;
        return true;
    }
    if ( (b = bombThreatDirection(location, Pos(-1, 0), ignore)) )
    {
        if ( bomb )
            *bomb = b;
        return true;
    }
    if ( (b = bombThreatDirection(location, Pos(0, 1), ignore)) )
    {
        if ( bomb )
            *bomb = b;
        return true;
    }
    if ( (b = bombThreatDirection(location, Pos(0, -1), ignore)) )
    {
        if ( bomb )
            *bomb = b;
        return true;
    }
    return false;
}

const Bomb * AIPlayer::bombThreatDirection(const Pos & location, const Pos & offset, const Bomb *ignore) const
{
    Pos pos = location;
    while(1)
    {
        pos += offset;
        char block = game->getMap().get(pos);
        if ( !Block::isSolid(block) )
            continue;
        const Bomb * b = game->getBomb(pos);
        if ( !b ) return 0;
        if ( !ignore || (ignore && ignore != b) )
            return manhattanDistance(location, b->getPos())
                    > b->getRadius() ? 0 : b;
        else
            continue;
    }
}

int AIPlayer::wallsToBeDestroyed() const
{
    int cnt = 0;
    if ( wallToBeDestroyedDirection(Pos(1, 0)) )
        cnt++;
    if ( wallToBeDestroyedDirection(Pos(-1, 0)) )
        cnt++;
    if ( wallToBeDestroyedDirection(Pos(0, 1)) )
        cnt++;
    if ( wallToBeDestroyedDirection(Pos(0, -1)) )
        cnt++;
    return cnt;
}

bool AIPlayer::wallToBeDestroyedDirection(const Pos &offset) const
{
    Pos pos = getPos();
    for ( int i = 1 ; i < getBombRadius() ; i++ )
    {
        pos += offset;
        char block = game->getMap().get(pos);
        if ( Block::isSolid(block) )
        {
            if ( block == Block::typeToSymbol(Block::DESTRUCTABLE) )
                return true;
            else
                return false;
        }
    }
    return false;
}

bool AIPlayer::bonusOpportunity(const Pos &offset) const
{
    Pos pos = getPos();
    while(1)
    {
        pos += offset;
        char block = game->getMap().get(pos);
        if ( Block::isSolid(block) )
            return false;
        else
        {
            if ( Block::symbolToType(block) >= Block::BONUS_BOMB
                 && Block::symbolToType(block) <= Block::BONUS_RADIUS )
            {
                return true;
            }
        }
    }
}

int AIPlayer::detonateBombsUtility() const
{
    return 0;
}

void AIPlayer::moveAction(const Pos &offset)
{
    game->movePlayer(*this, offset);
}

void AIPlayer::plantBombAction()
{
    game->plantTimedBomb(*this);
}
