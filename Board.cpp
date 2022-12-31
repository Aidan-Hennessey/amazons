// Definitions of Board methods

#include <bitset>
#include <vector>
#include <list>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "amazons.hpp"
#include "Board.hpp"
#include "UI.hpp"

// initializes board to start position
Board::Board() {
    //fill border with 1's
    for(int i=0; i < SETSIZE; i++) {
        if(i < BBWIDTH || i >= SETSIZE - BBWIDTH || i % BBWIDTH == 0 || i % BBWIDTH == BBWIDTH - 1) //is on border
            occupied.set(i);
        else
            occupied.reset(i);
    }
    //fill left and right starting positions
    if(BOARDWIDTH == 10) {
        left_amazons.set(16);
        left_amazons.set(19);
        left_amazons.set(49);
        left_amazons.set(58);
        right_amazons.set(85);
        right_amazons.set(94);
        right_amazons.set(124);
        right_amazons.set(127);
    }
    if(BOARDWIDTH == 8) {
        left_amazons.set(21);
        left_amazons.set(38);
        left_amazons.set(15);
        right_amazons.set(61);
        right_amazons.set(78);
        right_amazons.set(84);
    }
    if(BOARDWIDTH == 6) {
        left_amazons.set(10);
        left_amazons.set(22);
        right_amazons.set(53);
        right_amazons.set(41);
    }

    //include amazons in occupied
    occupied |= left_amazons;
    occupied |= right_amazons;
}

// copy constructor
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

    int dist = std::max(abs(hori_move_dist), abs(vert_move_dist));

    if(hori_move_dist > 0 && vert_move_dist > 0)
        incr = 1 + BBWIDTH;
    if(hori_move_dist > 0 && vert_move_dist == 0)
        incr = 1;
    if(hori_move_dist > 0 && vert_move_dist < 0)
        incr = 1 - BBWIDTH;
    if(hori_move_dist == 0 && vert_move_dist > 0)
        incr = BBWIDTH;
    if(hori_move_dist == 0 && vert_move_dist == 0)
        return false; // start and end are the same square
    if(hori_move_dist == 0 && vert_move_dist < 0)
        incr = -BBWIDTH;
    if(hori_move_dist < 0 && vert_move_dist > 0)
        incr = -1 + BBWIDTH;
    if(hori_move_dist < 0 && vert_move_dist == 0)
        incr = -1;
    if(hori_move_dist < 0 && vert_move_dist < 0)
        incr = -1 - BBWIDTH;

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
 * Returns the board resulting from making a certain move on this board,
 * without altering this board.
 *
 * Precondition: the move passed is a legal one
 *
 * Params:
 *     player - which player is trying to make the move (left or right)
 *     move - the move the player is trying to make
 * Returns:
 *     a Board object representing the board after the move is played
 */
Board Board::make_move_immutably(player_t player, move_t move) {
    Board moved_board(*this);
    assert(moved_board.make_move(player, move));
    return moved_board;
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
    int incrs[8] = INCRS_INIT; 

    for(int i=TOP_LEFT; i<=BOTTOM_RIGHT; i++) // for each square on the board
        if(has_amazon(player, i)) // if this player has an amazon there
            for(int j=0; j < 8; j++) // look at all the adjacent squares
                if(!occupied[i+incrs[j]]) // if one is unoccupied
                    return false; // the player has a move

    return true;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//////////////////////////                          ///////////////////////////////
//////////////////////////    UI RELATED METHODS    ///////////////////////////////
//////////////////////////                          ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

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
    char non_bit_board[BOARDWIDTH][BOARDWIDTH];

    for(int i=1; i<=BOARDWIDTH; i++) {
        for(int j=1; j<=BOARDWIDTH; j++) {
            non_bit_board[i-1][j-1] = tile_icon(this->get_tile_type(Point::rowcol_to_val(i, j)));
        }
    }

    print_board(non_bit_board);
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//////////////////////////                          ///////////////////////////////
//////////////////////////    AI RELATED METHODS    ///////////////////////////////
//////////////////////////                          ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

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
int Board::num_queen_connections(int index) {
    int count = 0;
    int incrs[8] = INCRS_INIT;

    for(int i=0; i<8; i++) {
        int j;
        for(j=1; !occupied[index + j * incrs[i]]; j++);
        count += j - 1;
    }

    return count;
}

/*
 * Helper for evaluate
 * Counts the number of moves that be made using an amazon at the passed square
 *
 * Params:
 *     index - the index in the bitboard of the square the amazon starts at
 * Return:
 *     an int - the number of distinct moves the amazon can make on this board
 */
int Board::count_amazon_moves(int index) {
    int count = 0;
    int incrs[8] = INCRS_INIT;
    int current;

    for(int i=0; i<8; i++) {
        for(int j=1; !occupied[(current = index + j * incrs[i])]; j++) {
            occupied.flip(index); // so the amazon doesn't get in the way of her arrow
            count += num_queen_connections(current);
            occupied.flip(index); // to avoid mutating the board
        }
    }

    return count;
}

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
int Board::find_num_moves(player_t player) {
    int count = 0;

    for(int i=TOP_LEFT; i<=BOTTOM_RIGHT; i++) {
        if(has_amazon(player, i)) {
            count += count_amazon_moves(i);
        }
    }

    return count;
} 

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
int Board::count_accessible_squares(player_t player) {
    std::bitset<SETSIZE> accesible;
    std::bitset<SETSIZE> fresh;
    std::bitset<SETSIZE> newly_accesible;
    int incrs[8] = INCRS_INIT;

    for(int i=TOP_LEFT; i<=BOTTOM_RIGHT; i++) {
        if(has_amazon(player, i)) {
            fresh.set(i);
        }
    }

    accesible |= fresh;

    while(fresh.any()) { // while we still have squares to search from
        for(int i=TOP_LEFT; i<=BOTTOM_RIGHT; i++) {
            if(fresh[i]) { // for each fresh square
                for(int j=0; j<8; j++) { // for each of its neighbors
                    if(!accesible[i + incrs[j]] && // if it's not already added
                        !occupied[i + incrs[j]]) { // and not occupied
                        newly_accesible.set(i + incrs[j]); // we've found a new square
                    }
                }
            }
        }
        accesible |= newly_accesible;
        fresh.reset();
        fresh |= newly_accesible;
        newly_accesible.reset();
    }

    return accesible.count();
}

/*
 * A heuristic which estimates which player the position is more favorable for
 * Positive values are better for left; negative are better for right
 * 
 * Params: none
 * Return:
 *     an int - the more positive, the better for left
 *              the more negative, the better for right
 */
int Board::evaluate() {
    int num_moves_diff = find_num_moves(LEFT) - find_num_moves(RIGHT);
    int accesible_squares_diff = count_accessible_squares(LEFT) - count_accessible_squares(RIGHT);

    return num_moves_diff + ALPHA * accesible_squares_diff;
}

// same as evaluate(), but prints additional info to stdout
int Board::evaluate_verbose() {
    int left_moves = find_num_moves(LEFT);
    int right_moves = find_num_moves(RIGHT);
    int num_moves_diff = left_moves - right_moves;
    int left_accesible = count_accessible_squares(LEFT);
    int right_accessible = count_accessible_squares(RIGHT);
    int accesible_squares_diff = left_accesible - right_accessible;
    int eval = num_moves_diff + ALPHA * accesible_squares_diff;

    printf("left moves: %i\n", left_moves);
    printf("right moves: %i\n", right_moves);
    printf("left access: %i\n", left_accesible);
    printf("right access: %i\n", right_accessible);
    printf("Final eval: %i\n", eval);
    return eval;
}

/*
 * Evaluates the position that would result from the playing of a passed move,
 * using a basic heuristic
 *
 * Params:
 *     player - the player who would make the move
 *     move - the move after which we want to evaluate
 * Return:
 *     an int - the evaluation of the resulting position
 */
int Board::evaluate(player_t player, move_t move) {
    Board edited_board = make_move_immutably(player, move);
    return edited_board.evaluate();
}

// same as evaluate(), but prints additional info to stdout
int Board::evaluate_verbose(player_t player, move_t move) {
    Board edited_board = make_move_immutably(player, move);
    return edited_board.evaluate_verbose();
}

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
std::list<Point> Board::queen_reachables(Point start) {
    std::list<Point> options;
    int incrs[8] = INCRS_INIT;
    int start_index = start.to_bbval();
    int current_index;

    for(int i=0; i<8; i++) {
        for(int j=1; !occupied[(current_index = start_index + j * incrs[i])]; j++) {
            options.push_back(Point(current_index));
        }
    }
    return options;
}

/*
 * Helper for get_moves()
 * constructs a list of the point locations of a player's amazons
 *
 * Params:
 *     player - the player whose amazons we want
 * Return:
 *     a list of points - the player's amazons (length 4 for 10x10 board)
 */
std::list<Point> Board::get_amazons(player_t player) {
    std::list<Point> amazons;

    for(int i=TOP_LEFT; i<=BOTTOM_RIGHT; i++) {
        if(has_amazon(player, i)) {
            amazons.push_back(Point(i));
        }
    }
    return amazons;
}

/*
 * constructs a vector of all possible moves for a player on this board
 *
 * Params: 
 *     player - the player for whom we're compiling moves
 * Returns:
 *     a vector of move_t objects - every single move the player can make
 */
std::vector<move_t> Board::get_moves(player_t player) {
    std::vector<move_t> moves;
    std::list<Point> amazons = get_amazons(player);
    std::list<Point> new_loc_options;
    std::list<Point> arrow_options;

    for(Point amazon : amazons) {
        new_loc_options = queen_reachables(amazon);
        occupied.flip(amazon.to_bbval());
        for(Point new_loc : new_loc_options) {
            arrow_options = queen_reachables(new_loc);
            for(Point arrow : arrow_options) {
                moves.push_back({amazon, new_loc, arrow});
            }
        }
        occupied.flip(amazon.to_bbval());
    }
    return moves;
}

/*
 * OLD AI METHOD
 * determines the best next move for a player in the current position,
 * using the basic heuristic
 *
 * Params:
 *     player - the player for whom we want to find the best move
 * Return:
 *     a move_t - the ai's best guess at the optimal move for player
 */
move_t Board::best_move(player_t player) {
    std::vector<move_t> moves = this->get_moves(player);
    int best_eval = worst_eval(player);
    move_t best_move;

    for(move_t move : moves) {
        int eval = this->evaluate(player, move);
        if(first_better(player, eval, best_eval)) {
            best_eval = eval;
            best_move = move;
        }
    }
    return best_move;
}