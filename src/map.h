#ifndef MAP_H
#define MAP_H

#include <vector>
#include <set>

#include "block.h"
#include "player.h"
#include "pos.h"
#include "game_gui.h"

/**
 * @brief The Map class represents a game board.
 */
class Map
{
public:

    static const int maxWidth = 47;
    static const int maxHeight = 23;

    /**
     * @brief The MapLoadException struct describes an error that may
     * occur while loading a game board from a file.
     */
    struct MapLoadException
    {
        /**
         * @brief The Type enum represents a type of a loading failure.
         */
        enum Type {READ_FAILURE, INVALID_MAP};

        /**
         * @brief Constructs a MapLoadException with given Type.
         * @param t The type of the MapLoadException.
         */
        MapLoadException(Type t):type(t){}

        /**
         * @brief type The type of the MapLoadException.
         */
        Type type;
    };

    /**
     * @brief Creates an empty game board.
     */
    Map();

    /**
     * @brief Loads a game board from the file.
     * @param file The file system path to the file.
     * @param playerPos Outputs a position of the player.
     * @param enemyPos Outputs a position of the enemy.
     * @param trapsPos Outputs positions of traps.
     * @throws MapLoadException
     */
    Map(const char * file,
        Pos & playerPos,
        Pos & enemyPos,
        std::set<Pos> & trapsPos, GameGUI *gameGUI=0);

    /**
     * @brief Loads a game board from the file.
     * @param file The file system path to the file.
     * @param playerPos Outputs a position of the player.
     * @param enemyPos Outputs a position of the enemy.
     * @param trapsPos Outputs positions of traps.
     * @throws MapLoadException
     */
    void load(const char * file,
              Pos & playerPos,
              Pos & enemyPos,
              std::set<Pos> & trapsPos, GameGUI *gameGUI=0);

    ~Map();

    /**
     * @brief Draws the game board to the screen.
     */
    void drawTUI() const;
    void drawGUI() const;

    /**
     * @brief Returns a symbol, a graphical representation of an object in
     * the game board, at given position. The symbol can be modified by
     * the caller.
     * @return A symbol, a graphical representation of an object in
     * the game board, at given position.
     */
    char & at(const Pos & pos);

    /**
     * @brief Returns a symbol, a graphical representation of an object in
     * the game board, at given position.
     * @return A symbol, a graphical representation of an object in
     * the game board, at given position.
     */
    char get(const Pos & pos) const;

    /**
     * @brief Returns the width of the game board.
     * @return The width of the game board.
     */
    int width() const;

    /**
     * @brief Returns the height of the game board.
     * @return The height of the game board.
     */
    int height() const;

    /**
     * @brief Checks whether the given position is a valid position in
     * the game board.
     * @return true if the position is valid, false otherwise.
     */
    bool withinBounds(const Pos & pos) const;
private:
    std::vector<char*> mMap;

    int mWidth;

    void clearMap();
    bool validOuterBlock(char symbol) const;
    bool validInnerBlock(char symbol) const;

    GameGUI * mGameGUI;
};

#endif
