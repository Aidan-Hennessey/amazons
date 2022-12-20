// for each move we're considering:
// search all its children ~500
// mark the top 20, 20, 10, 10, 5, 5 as moves we're considering (for depth d)
// 

// 

// Things that could factor into heuristic:
// union-find reachable squares for me vs for opponent
// number of possible moves for me vs for opponent
// ^^ sum of squares across queens of either of those
// weight them by a function of how many turns we've had

#include <bitset>
#include "Board.hpp"

// empirically chosen parameter
#define ALPHA 50
#define BIGNUM 999999 //greater than any possible evaluation value
#define worst_eval(p) ((p == left) ? -BIGNUM : BIGNUM)
#define first_better(p, a, b) ((p == left) ? a > b : a < b)

/*
 * Finds the number of squares a chess queen could move to if it was on the 
 * square corresponding to index
 *
 * Params:
 *     index - the index in the bitboard of the queen's starting square
 * Returns:
 *     A count of how many distinct legal moves the queen would have
 */
int Board::num_queen_connections(int index) {
    int count = 0;
    int incrs[8] = {11, 12, 13, 1, -11, -12, -13, -1};

    for(int i=0; i<8; i++) {
        int j;
        for(j=1; !occupied[index + j * incrs[i]]; j++);
        count += j - 1;
    }

    return count;
}

/*
 * Counts the number of moves that be made using an amazon at the passed square
 *
 * Params:
 *     index - the index in the bitboard of the square the amazon starts at
 * Return:
 *     an int - the number of distinct moves the amazon can make on this board
 */
int Board::count_amazon_moves(int index) {
    int count = 0;
    int incrs[8] = {11, 12, 13, 1, -11, -12, -13, -1};
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
 * Finds the number of distinct legal moves a certain player can make on the 
 * current board
 *
 * Params:
 *     player - which player's moves we're counting (left or right)
 * Return:
 *     an int - a count of the number of distinct moves this player could make
 *              if going next
 */
int Board::find_num_moves(player_t player) {
    int count = 0

    for(int i=13; i<=130;i++) {
        if(has_amazon(player, i)) {
            count += count_amazon_moves(i);
        }
    }

    return count;
} 

/*
 * Returns a count of the number of squares a player can reach by repeatedly
 * moving their amazons like chess kings
 *
 * Params:
 *     player - the player who accesibility we're measuring
 * Return:
 *     an int - the count of how many squares are accessible to this player
 */
int Board::count_accessible_squares(player_t player) {
    std::bitset accesible<144>;
    std::bitset fresh<144>;
    std::bitset newly_accesible<144>;
    int incrs[8] = {11, 12, 13, 1, -11, -12, -13, -1};
    int prev_count;
    int cur_count = 4;

    for(int i=13; i<=130;i++) {
        if(has_amazon(player, i)) {
            fresh.set(i);
        }
    }

    accesible |= fresh;

    while(!fresh.empty()) { // while we still have squares to search from
        for(int i=13; i<=130; i++) {
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
int Board::evaluation() {
    int num_moves_diff = find_num_moves(left) - find_num_moves(right);
    int accesible_squares_diff = count_accessible_squares(left) - count_accessible_squares(right);

    return num_moves_diff + ALPHA * accesible_squares_diff;
}

std::list<Board> Board::get_moves(player_t player) {
    std::list<Board> moves;

    
}

Board best_move(player_t player, std::list<Board> moves) {
    int best_eval = worst_eval(player);
    Board best_move;

    for(const auto& move : moves) {
        int eval = move.evaluation();
        if(first_better(eval, best_eval, player)) {
            best_eval = eval;
            best_move = move;
        }
    }

    return best_move;
}

void ai_move(player_t player, Board& board) {
    std::list<Board> moves = board.get_moves(player);
    board = best_move(player, moves)
}