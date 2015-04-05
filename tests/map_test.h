#include "../map.h"

#include <assert.h>

void map_test()
{
    try{
        Map map("tests/map/test1.unix");
        map.load("tests/map/test1.dos");
        map.load("tests/map/test1.mac");

        try{
            map.load("tests/map/test2.unix"); /* inconsistent size */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test3.unix"); /* too tiny map */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test4.unix"); /* no player */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test5.unix"); /* two players */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test6.unix"); /* invalid object */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test_not_existent.unix"); /* invalid file */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::READ_FAILURE );
        }

        try{
            map.load("tests/map/test7.unix"); /* side hole */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }

        try{
            map.load("tests/map/test8.unix"); /* top hole */
            assert( false );
        }catch(const Map::MapLoadException & e) {
            assert ( e.type == Map::MapLoadException::INVALID_MAP );
        }
    } catch (const Map::MapLoadException & e)
    { assert ( false ); }

}
