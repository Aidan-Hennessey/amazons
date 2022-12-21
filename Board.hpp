#ifndef BOARD_H
#define BOARD_H

#include <bitset>
#include <list>
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
    //////////////////  CONSTRUCTORS  /////////////////////
    Board();
    Board(const Board& to_copy);

    //////////////////  GENERAL METHODS  //////////////////

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
    bool queen_connected(Point start, Point end) const;

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
    bool move_is_legal(player_t player, move_t move);

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
    bool make_move(player_t player, move_t move);

    /*
     * Determines if the passed player has any legal moves
     *
     * Params:
     *     player - which player we're checking for moves
     * Return:
     *     a bool - true if the player is out of moves, false if they can still move
     */
    bool no_moves(player_t player);

    //////////////////  UI RELATED METHODS  ////////////////////

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

    ////////////////////  AI RELATED METHODS  /////////////////////

    /*
     * Helper for evaluate
     * Finds the number of squares a chess queen could move to if it was on the 
     * square corresponding to index
     *
     * Note: could just call queen_reachables and take length, but doesn't for
     *       performance reasons. Needs to be fast so AI can consider as many
     *       moves as possible
     *
     * Params:
     *     index - the index in the bitboard of the queen's starting square
     * Returns:
     *     A count of how many distinct legal moves the queen would have
     */
    int num_queen_connections(int index);

    /*
     * Helper for evaluate
     * Counts the number of moves that be made using an amazon at the passed square
     *
     * Params:
     *     index - the index in the bitboard of the square the amazon starts at
     * Return:
     *     an int - the number of distinct moves the amazon can make on this board
     */
    int count_amazon_moves(int index);

    /*
     * Helper for evaluate
     * Finds the number of distinct legal moves a certain player can make on the 
     * current board
     *
     * Note: could just call get_moves and take length, but doesn't for
     *       performance reasons. Needs to be fast so AI can consider as many
     *       moves as possible
     *
     * Params:
     *     player - which player's moves we're counting (left or right)
     * Return:
     *     an int - a count of the number of distinct moves this player could make
     *              if going next
     */
    int find_num_moves(player_t player);

    /*
     * Helper for evaluate
     * Returns a count of the number of squares a player can reach by repeatedly
     * moving their amazons like chess kings
     *
     * Params:
     *     player - the player whose accesibility we're measuring
     * Return:
     *     an int - the count of how many squares are accessible to this player
     */
    int count_accessible_squares(player_t player);

    /*
     * A heuristic which estimates which player the position is more favorable for
     * Positive values are better for left; negative are better for right
     * 
     * Params: none
     * Return:
     *     an int - the more positive, the better for left
     *              the more negative, the better for right
     */
    int evaluate();

    int evaluate_verbose();

    /*
     * Evaluates the position that would result from the playing of a passed move
     *
     * Params:
     *     player - the player who would make the move
     *     move - the move after which we want to evaluate
     * Return:
     *     an int - the evaluation of the resulting position
     */
    int evaluate(player_t player, move_t move);

    int evaluate_verbose(player_t player, move_t move);

    /*
     * Helper for get_moves()
     * Constructs a list of points that a chess queen could move to starting from
     * the passed square
     *
     * Params:
     *     start - the starting square
     * Return:
     *     a list of points - the points a queen at start could move to
     */
    std::list<Point> queen_reachables(Point start);

    /*
     * Helper for get_moves()
     * constructs a list of the point locations of a player's amazons
     *
     * Params:
     *     player - the player whose amazons we want
     * Return:
     *     a list of points of length 4 - the player's 4 amazons
     */
    std::list<Point> get_amazons(player_t player);

    /*
     * constructs a list of all possible moves for a player on this board
     *
     * Params: 
     *     player - the player for whom we're compiling moves
     * Returns:
     *     a list of move_t objects - every single move the player can make
     */
    std::list<move_t> get_moves(player_t player);

    /*
     * determines the best next move for a player in the current position
     *
     * Params:
     *     player - the player for whom we want to find the best move
     * Return:
     *     a move_t - the ai's best guess at the optimal move for player
     */
    move_t best_move(player_t player);
};

#endif