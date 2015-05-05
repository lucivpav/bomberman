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
    updateBestAction(Action::IDLE, action, utility);
    if ( utility == 0 ) // bomb threat
    {
        if ( canFleeDirection(Pos(1, 0)) )
            return MOVE_RIGHT;
        if ( canFleeDirection(Pos(-1, 0)) )
            return MOVE_LEFT;
        if ( canFleeDirection(Pos(0, 1)) )
            return MOVE_DOWN;
        if ( canFleeDirection(Pos(0, -1)) )
            return MOVE_UP;
        updateBestAction(DETONATE_BOMBS, action, utility);
        return action;
    }
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

int AIPlayer::idleUtility() const
{
    if ( !isBombSafe(getPos()) )
        return 0;
    return 200 - manhattanDistance(getPos(), enemy->getPos());
}

int AIPlayer::moveUtility(const Pos &offset) const
{
    if ( !game->canMovePlayer(getPos() + offset) )
        return 0;
    return 200 - manhattanDistance(getPos() + offset, enemy->getPos());
}

int AIPlayer::plantBombUtility() const
{
    if ( !game->canPlantBomb(*this) || !canFlee() )
        return 0;
    int dist = airDistance(getPos(), enemy->getPos());
    if ( dist < getBombRadius() )
        return 400 - dist;
    return wallsToBeDestroyed() * 160;
}

bool AIPlayer::canFlee() const
{
    return canFleeDirection(Pos(-1, 0))
           || canFleeDirection(Pos(1, 0))
           || canFleeDirection(Pos(0, 1))
           || canFleeDirection(Pos(0, -1));
}

bool AIPlayer::canFleeDirection(Pos offset) const
{
    int bombRadius = getBombRadius();
    Pos playerPos = getPos();
    for ( int i = 0 ; ; i++ )
    {
        playerPos += offset;
        Pos side = Pos(offset.y, offset.x) + playerPos;
        Pos side2 = Pos(offset.y * -1, offset.x * -1) + playerPos;

        if ( !game->canMovePlayer(playerPos) )
            return false;
        if ( isBombSafe(playerPos) )
        {
            char symbol = game->getMap().get(getPos());
            if ( symbol == Block::typeToSymbol(Block::TIMED_BOMB)
                 || symbol == Block::typeToSymbol(Block::REMOTE_BOMB) )
            {
                if ( i > bombRadius )
                    return true;
            }
            return true;
        }
        if ( (game->canMovePlayer(side) && isBombSafe(side))
             || (game->canMovePlayer(side2) && isBombSafe(side2)) )
            return true;
    }
}

bool AIPlayer::isBombSafe(Pos location) const
{
    Block::Type curBlock = Block::symbolToType(game->getMap().get(location));
    if ( curBlock != Block::TIMED_BOMB
         && curBlock != Block::REMOTE_BOMB
         && bombSafeDirection(location, Pos(1, 0))
         && bombSafeDirection(location, Pos(-1, 0))
         && bombSafeDirection(location, Pos(0, 1))
         && bombSafeDirection(location, Pos(0, -1)) )
        return true;
    return false;
}

bool AIPlayer::bombSafeDirection(const Pos & location, const Pos & offset) const
{
    Pos pos = location;
    for ( int i = 1 ; ; i++ )
    {
        pos += offset;
        char block = game->getMap().get(pos);
        if ( !Block::isSolid(block) )
            continue;

        if ( block == Block::typeToSymbol(Block::TIMED_BOMB) )
        {
            const TimedBomb & b = game->getTimedBomb(pos);
            return i > b.getRadius();
        }
        else if (block == Block::typeToSymbol(Block::REMOTE_BOMB) )
        {
            return !enemy->hasRemoteBomb(pos);
        }
        else /* no bomb */
            return true;
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
