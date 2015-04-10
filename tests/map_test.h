#include "../src/map.h"

#include <assert.h>

void map_test()
{
    Pos pos;
    try{
        Map map("tests/map/test1.unix", pos);
        map.load("tests/map/test1.dos", pos);
        map.load("tests/map/test1.mac", pos);

        try{
            map.load("tests/map/test2.unix", pos); /* inconsistent size */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test3.unix", pos); /* too tiny map */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test4.unix", pos); /* no player */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test5.unix", pos); /* two players */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test6.unix", pos); /* invalid object */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test_not_existent.unix", pos); /* invalid file */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::READ_FAILURE );
        }

        try{
            map.load("tests/map/test7.unix", pos); /* side hole */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test8.unix", pos); /* top hole */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }
    } catch (const Map::MapLoadException & e)
    { assert ( false ); }

}
