#include <bitset>
#include <list>
#include <vector>
#include <assert.h>
#include "Board.hpp"
#include "amazons.hpp"
#include "ai.hpp"
#include "MoveTree.hpp"

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
 * Evaluates the position that would result from the playing of a passed move
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
 * constructs a list of all possible moves for a player on this board
 *
 * Params: 
 *     player - the player for whom we're compiling moves
 * Returns:
 *     a list of move_t objects - every single move the player can make
 */
std::vector<move_t> Board::get_moves(player_t player) {
    std::vector<move_t> moves;
    std::list<Point> amazons = get_amazons(player);
    std::list<Point> new_loc_options;
    std::list<Point> arrow_options;

    for(const auto& amazon : amazons) {
        new_loc_options = queen_reachables(amazon);
        occupied.flip(amazon.to_bbval());
        for(const auto& new_loc : new_loc_options) {
            arrow_options = queen_reachables(new_loc);
            for(const auto& arrow : arrow_options) {
                moves.push_back({amazon, new_loc, arrow});
            }
        }
        occupied.flip(amazon.to_bbval());
    }
    return moves;
}

/*
 * determines the best next move for a player in the current position
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

    for(const auto& move : moves) {
        int eval = this->evaluate(player, move);
        if(first_better(player, eval, best_eval)) {
            best_eval = eval;
            best_move = move;
        }
    }
    return best_move;
}

/*
 * The ai makes a move
 *
 * Params:
 *     player - which player the ai is moving for
 *     board - the board on which to make a move
 * Return: none
 */
move_t ai_move(Board& board, player_t player) {
    MoveTree tree(board, player);

    return tree.make_move(board);
}