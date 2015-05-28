#include "ai_player.h"

#include "game.h"
#include "block.h"

#include <cassert>
#include <queue>
#include <set>
#include <map>
#include <algorithm>

AIPlayer::AIPlayer(Game *g, Player *p, const Pos &pos, int lives, int bombs)
    :Player(g, pos, lives, bombs),
      enemy(p), mIdleCountdown(2000), mMoveCountdown(100), mIdle(false)
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
    if ( mIdleCountdown.expired(mIdle ? 2000 : 1500) )
        mIdle = !mIdle;

    if ( mIdle )
    {
        if ( !bombThreat(getPos()) )
            return;
        else
            mIdle = false;
    }

    if ( !mIdle )
    {
        if ( !mMoveCountdown.expired() )
            return;
    }

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
    mPathfinderHint.clear();
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
        mPath = mCandidatePath;
    }
}

/* check for curPos safety before calling this */
int AIPlayer::idleUtility()
{
    return ( 201 - distanceToEnemy(getPos(), &mPathfinderHint) );
}

int AIPlayer::moveUtility(const Pos &offset)
{
    Pos pos = getPos() + offset;
    const Trap * trap = game->trapAt(pos);
    if ( !game->canMovePlayer(pos)
         || bombThreat(pos)
         || (trap && trap->isOpen()) )
        return 0;
    if ( bonusOpportunity(offset) )
        return 400;
    return 201 - distanceToEnemy(getPos() + offset, &mPathfinderHint);
}

int AIPlayer::plantBombUtility()
{
    if ( !game->canPlantBomb(*this)
         || !canFlee(Bomb(0, getPos(), getBombRadius())) )
        return 0;
    int dist = Pos::manhattanDistance(getPos(), enemy->getPos());
    if ( dist < getBombRadius() && (rand() % dist == 0) )
        return 400 - dist;
    return 200 - distanceToEnemy(getPos(), &mPathfinderHint)
            + (wallsToBeDestroyed() * 50);
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
        const Trap * trap;

        if ( !game->canMovePlayer(playerPos)
             || ((trap = game->trapAt(playerPos)) && trap->isOpen()) )
            return 0;
        const Bomb * threat2;
        if ( bombThreat(playerPos, &threat2, &threat) )
            return 0;
        {
            if ( playerPos.x == threat.getPos().x
                 || playerPos.y == threat.getPos().y )
            {
                if ( Pos::manhattanDistance(playerPos, threat.getPos()) > getBombRadius() )
                    return steps;
            }
            else
                return steps;
        }
        if ( (game->canMovePlayer(side)
              && !bombThreat(side)
              && (!(trap = game->trapAt(side)) || !trap->isOpen()))
             ||
             (game->canMovePlayer(side2)
              && !bombThreat(side2)
              && (!(trap = game->trapAt(side2)) || !trap->isOpen())) )
            return steps + 1;
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
    else if ( game->isFlameAt(location) )
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
        if ( !game->withinBounds(pos) )
            return 0;
        char block = game->getMap().get(pos);
        if ( !Block::isSolid(block) )
            continue;
        const Bomb * b = game->getBomb(pos);
        if ( !b ) return 0;
        if ( !ignore || (ignore && ignore != b) )
            return Pos::manhattanDistance(location, b->getPos())
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
        if ( !game->withinBounds(pos) )
            return false;
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
        if ( !game->withinBounds(pos) )
            return false;
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

int AIPlayer::distanceToEnemy(Pos from, std::vector<Pos> * hint) const
{
    using std::make_pair;
    typedef std::pair<int, Pos> pqe;
    std::priority_queue<pqe, std::vector<pqe>, std::greater<pqe>> open;
    std::set<Pos> closed;
    std::map<Pos, Pos> prev;
    std::map<Pos, int> dist;
    std::set<Pos> hintSet;

    if ( hint )
        for ( const auto & h : *hint )
            hintSet.insert(h);

    open.emplace(make_pair(Pos::manhattanDistance(from, enemy->getPos()),
                           from));
    dist.insert(make_pair(from, 0));

    while ( !open.empty() )
    {
        Pos current = open.top().second;
        open.pop();

        if ( hint && hintSet.count(current) )
        {
            auto it = std::find(hint->begin(), hint->end(), current);
            assert ( it != hint->end() );
            hint->erase(it, hint->end());
            reconstructPath(from, current, prev, *hint);

            mCandidatePath = *hint;
            return hint->size()-1;
        }
        if ( current == enemy->getPos() )
        {
            std::vector<Pos> path;
            reconstructPath(from, enemy->getPos(), prev, path);

            if ( hint )
                *hint = path;
            mCandidatePath = path;
            return path.size()-1;
        }

        Pos expanded[4];
        expanded[0] = current + Pos(-1, 0);
        expanded[1] = current + Pos(1, 0);
        expanded[2] = current + Pos(0, -1);
        expanded[3] = current + Pos(0, 1);
        for ( int i = 0 ; i < 4 ; i++ )
        {
            Pos & expand = expanded[i];
            if ( closed.count(expand) )
                continue;
            if ( game->getMap().get(expand) == Block::typeToSymbol(Block::WALL) )
            {
                closed.insert(expand);
                continue;
            }
            int cost = dist[current] + 1;
            if ( !dist.count(expand) || cost < dist[expand] )
            {
                dist.insert(std::make_pair(expand, cost));
                prev.insert(std::make_pair(expand, current));
                open.emplace(cost
                             + Pos::manhattanDistance(expand,
                                                      enemy->getPos()),
                             expand);
            }
        }
        closed.insert(current);
    }
    assert ( false );
    return -1;
}

void AIPlayer::reconstructPath(const Pos & from,
                               const Pos & to,
                               const std::map<Pos, Pos> & prev,
                               std::vector<Pos> & path) const
{
    path.push_back(to);
    Pos toBeAdded = to;
    while ( 1 )
    {
        if ( toBeAdded == from )
            break;
        toBeAdded = prev.find(toBeAdded)->second;
        path.push_back(toBeAdded);
    }
}
