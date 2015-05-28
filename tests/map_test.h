#include "../src/map.h"

#include <assert.h>

void map_test()
{
    Pos pos;
    Pos enemyPos;
    std::set<Pos> trapsPos;
    try{
        Map map("tests/map/test1.unix", pos, enemyPos, trapsPos);
        map.load("tests/map/test1.dos", pos, enemyPos, trapsPos);
        map.load("tests/map/test1.mac", pos, enemyPos, trapsPos);

        try{
            map.load("tests/map/test2.unix", pos, enemyPos, trapsPos); /* inconsistent size */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test3.unix", pos, enemyPos, trapsPos); /* too tiny map */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test4.unix", pos, enemyPos, trapsPos); /* no player */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test5.unix", pos, enemyPos, trapsPos); /* two players */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test6.unix", pos, enemyPos, trapsPos); /* invalid object */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test_not_existent.unix", pos, enemyPos, trapsPos); /* invalid file */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::READ_FAILURE );
        }

        try{
            map.load("tests/map/test7.unix", pos, enemyPos, trapsPos); /* side hole */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test8.unix", pos, enemyPos, trapsPos); /* top hole */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test9.unix", pos, enemyPos, trapsPos); /* no enemy */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test10.unix", pos, enemyPos, trapsPos); /* two enemies */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }
    } catch (const Map::MapLoadException & e)
    { assert ( false ); }

}
