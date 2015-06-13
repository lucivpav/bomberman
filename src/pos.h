#ifndef POS_H
#define POS_H

#include <cmath>

/**
 * @brief The Pos struct represents a two dimensional position in a space.
 */
struct Pos
{
    /**
     * @brief Does not set the coordinates to any value. The caller is
     * responsible to setting the coordinates manually.
     */
    Pos();

    /**
     * @brief Sets the coordinates to the specified values.
     * @param x The x coordinate.
     * @param y The y coordinate.
     */
    Pos(int x, int y);

    /**
     * @brief The x coordinate.
     */
    int x;

    /**
     * @brief The y coordinate.
     */
    int y;

    /**
     * @brief Adds a Pos to current coordinates without modifying
     * the previous position. This effectively adds
     * the respecive coordinates together.
     * @param pos The coordinates to be added.
     * @return The new position.
     */
    Pos operator+(const Pos & pos) const;

    /**
     * @brief Subtracts a Pos from current coordinates without modifying
     * the previous position. This effectively
     * subtracts the respecive coordinates from each other.
     * @param pos The coordinates to be subtracted..
     * @return The new position.
     */
    Pos operator-(const Pos & pos) const;

    /**
     * @brief Adds a Pos to current coordinates and overrides the
     * previous position. This effectively adds
     * the respecive coordinates together.
     * @param pos The coordinates to be added.
     * @return The new position.
     */
    Pos & operator+=(const Pos & pos);

    /**
     * @brief Subtracts a Pos from current coordinates and overrides the
     * previous position. This effectively
     * subtracts the respecive coordinates from each other.
     * @param pos The coordinates to be subtracted..
     * @return The new position.
     */
    Pos & operator-=(const Pos & pos);

    /**
     * @brief Checks whether two positions are equal.
     * @param pos The Pos to be compared with.
     * @return true if the positions are equal, false otherwise.
     */
    bool operator==(const Pos & pos) const;

    /**
     * @brief Checks whether two positions are different.
     * @param pos The Pos to be compared with.
     * @return true if the positions are different, false otherwise.
     */
    bool operator!=(const Pos & pos) const;

    /**
     * @brief Checks whether the position is smaller than another one.
     * This can be used for sorting Pos objects. The underlying implementation
     * is not important.
     * @param pos The Pos to be compared with.
     * @return true if the the position is smaller than the one being
     * compared against.
     */
    bool operator<(const Pos & pos) const;

    /**
     * @brief Computes a manhattan distance between two positions.
     * @return The manhattan distance between two points.
     */
    static int manhattanDistance(const Pos & a, const Pos & b);
};

#endif
