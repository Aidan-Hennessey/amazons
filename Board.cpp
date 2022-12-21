#include <bitset>
#include "amazons.hpp"
#include "Board.hpp"
#include "UI.hpp"

Board::Board() {
    //fill border with 1's
    for(int i=0; i < 144; i++) {
        if(i < 12 || i >= 132 || i % 12 == 0 || i % 12 == 11) //is on border
            occupied.set(i);
        else(occupied.reset(i));
    }
    //fill left and right starting positions
    left_amazons.set(16);
    left_amazons.set(19);
    left_amazons.set(49);
    left_amazons.set(58);
    right_amazons.set(85);
    right_amazons.set(94);
    right_amazons.set(124);
    right_amazons.set(127);

    //include amazons in occupied
    occupied |= left_amazons;
    occupied |= right_amazons;
}

Board::Board(const Board& to_copy) {
    // using |= not = so as to avoid ailiasing weirdness
    this->occupied.reset();
    this->occupied |= to_copy.occupied;
    this->left_amazons.reset();
    this->left_amazons |= to_copy.left_amazons;
    this->right_amazons.reset();
    this->right_amazons |= to_copy.right_amazons;
}

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
bool Board::queen_connected(Point start, Point end) const {
    int start_index = start.to_bbval();
    int incr;

    int hori_move_dist = end.get_col() - start.get_col();
    int vert_move_dist = end.get_row() - start.get_row();

    if((abs(hori_move_dist)) != (abs(vert_move_dist)) && hori_move_dist * vert_move_dist != 0)
        return false; // path is not straight line

    int dist = max(abs(hori_move_dist), abs(vert_move_dist));

    if(hori_move_dist > 0 && vert_move_dist > 0)
        incr = 13;
    if(hori_move_dist > 0 && vert_move_dist == 0)
        incr = 1;
    if(hori_move_dist > 0 && vert_move_dist < 0)
        incr = -11;
    if(hori_move_dist == 0 && vert_move_dist > 0)
        incr = 12;
    if(hori_move_dist == 0 && vert_move_dist == 0)
        return false; // start and end are the same square
    if(hori_move_dist == 0 && vert_move_dist < 0)
        incr = -12;
    if(hori_move_dist < 0 && vert_move_dist > 0)
        incr = 11;
    if(hori_move_dist < 0 && vert_move_dist == 0)
        incr = -1;
    if(hori_move_dist < 0 && vert_move_dist < 0)
        incr = -13;

    for(int i=1; i <= dist; i++) {
        if(occupied[start_index + incr * i]) {
            Point occupied_point(start_index + incr * i);
            return false; // path is obstructed
        }
    }

    return true;
}

/*
 * Determines whether a given move is legal
 *
 * Params:
 *     player - which player is trying to make the move (left or right)
 *     move - the move to determine the legality of
 * Returns:
 *     a bool - true if the move is legal, false otherwise
 */
bool Board::move_is_legal(player_t player, move_t move) {
    bool legal;
    int start_index = move.old_loc.to_bbval();

    if(!has_amazon(player, start_index)) // player does not have amazon there
        return false;

    occupied.flip(start_index); // so it doesn't think the queen's old position is blocking the arrow
    legal = queen_connected(move.old_loc, move.new_loc) && queen_connected(move.new_loc, move.arrow);
    occupied.flip(start_index); // so that this method doesn't mutate the board
    return legal;
}

/*
 * if the move is legal, mutates the board accordingly and returns true
 * else, does not mutate the board and returns false
 *
 * Params:
 *     player - which player is trying to make the move (left or right)
 *     move - the move the player is trying to make
 * Returns:
 *     a bool - true if the move is legal, false otherwise
 */
bool Board::make_move(player_t player, move_t move) {
    if(!move_is_legal(player, move))
        return false;

    int start = move.old_loc.to_bbval();
    int finish = move.new_loc.to_bbval();
    int to_burn = move.arrow.to_bbval();

    flip_amazon(player, start);
    flip_amazon(player, finish);
    occupied.reset(start);
    occupied.set(finish);
    occupied.set(to_burn);

    return true;
}

/*
 * Determines if the passed player has any legal moves
 *
 * Params:
 *     player - which player we're checking for moves
 * Return:
 *     a bool - true if the player is out of moves, false if they can still move
 */
bool Board::no_moves(player_t player) {
    // the increments to get to adjacent squares
    int incrs[8] = {-13, -12, -11, -1, 1, 11, 12, 13}; 

    for(int i=13; i<=130; i++) // for each square on the board
        if(has_amazon(player, i)) // if this player has an amazon there
            for(int j=0; j < 8; j++) // look at all the adjacent squares
                if(!occupied[i+incrs[j]]) // if one is unoccupied
                    return false; // the player has a move

    return true;
}

/*
 * returns the state of the square corresponding to the passed index
 *
 * Params:
 *     index - the index in the bitboards of tile to return the state of
 * Returns:
 *     a tile_state_t enum specifying whether the tile is a left amazon, 
 *     right amazon, burnt, or open
 */
tile_state_t Board::get_tile_type(int index) {
    if(!occupied[index]) return open;
    if(left_amazons[index]) return lamazon;
    if(right_amazons[index]) return ramazon;
    return burnt;
}

/*
 * prints the board
 * Also prints the key for comprehending the board
 *
 * Params: none
 * Returns: none
 */
void Board::print() {
    char non_bit_board[10][10];

    for(int i=1; i<=10; i++) {
        for(int j=1; j<=10; j++) {
            non_bit_board[i-1][j-1] = tile_icon(this->get_tile_type(Point::rowcol_to_val(i, j)));
        }
    }

    print_board(non_bit_board);
}