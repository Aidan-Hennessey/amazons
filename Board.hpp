#ifndef BOARD_H
#define BOARD_H

#include <bitset>
#include "amazons.hpp"

#define has_amazon(p, v) ((p == left) ? left_amazons[v] : right_amazons[v])
#define flip_amazon(p, v) ((p == left) ? left_amazons.flip(v) : right_amazons.flip(v))
#define abs(a) ((a < 0) ? -a : a)
#define max(a, b) ((a > b) ? a : b)

/*
 * the board, internally represented as 3 bitboards - one for occupied squares, 
 * and one for each player's set of amazons
 */
class Board {
    std::bitset<144> occupied;
    std::bitset<144> left_amazons;
    std::bitset<144> right_amazons;

    public:
    Board();

    /*
     * helper for move_is_legal()
     * determines whether a chess queen could move from start to end
     *
     * Params:
     *     start - the starting position of the queen
     *     end - the queen's target square
     *
     * Returns:
     *     a bool - false if not a straight line or if there is an impediment, true otherwise
     */
    bool queen_connected(Point start, Point end);

    /*
     * Determines whether a given move is legal
     *
     * Params:
     *     player - which player is trying to make the move (left or right)
     *     amazon_start - the location of the amazon the player is trying to move
     *     amazon_finish - the location the player wishes to move the amazon
     *     arrow - the location of the tile the player wishes to burn
     * Returns:
     *     a bool - true if the move is legal, false otherwise
     */
    bool move_is_legal(player_t player, Point amazon_start, Point amazon_finish, Point arrow);

    /*
     * if the move is legal, mutates the board accordingly and returns true
     * else, does not mutate the board and returns false
     *
     * Params:
     *     player - which player is trying to make the move (left or right)
     *     amazon_start - the location of the amazon the player is trying to move
     *     amazon_finish - the location the player wishes to move the amazon
     *     arrow - the location of the tile the player wishes to burn
     * Returns:
     *     a bool - true if the move is legal, false otherwise
     */
    bool make_move(player_t player, Point amazon_start, Point amazon_finish, Point arrow);

    /*
     * Determines if the passed player has any legal moves
     *
     * Params:
     *     player - which player we're checking for moves
     * Return:
     *     a bool - true if the player is out of moves, false if they can still move
     */
    bool no_moves(player_t player);

    /*
     * returns the state of the square corresponding to the passed index
     *
     * Params:
     *     index - the index in the bitboards of tile to return the state of
     * Returns:
     *     a tile_state_t enum specifying whether the tile is a left amazon, 
     *     right amazon, burnt, or open
     */
    tile_state_t get_tile_type(int index);

    /*
     * prints the board
     * Also prints the key for comprehending the board
     *
     * Params: none
     * Returns: none
     */
    void print();
};

#endif