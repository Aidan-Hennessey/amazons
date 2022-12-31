#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <unordered_set>
#include "amazons.hpp"
#include "Board.hpp"
#include "MoveTree.hpp"

/*
 * This is the constructor used from outside the class to construct an empty tree
 *
 * Params:
 *     board - the board state of the tree
 *     player - whose turn it is to move
 */
MoveTree::MoveTree(Board board, player_t player) {
    this->parent = this;
    // there is no previous move

    this->board = Board(board);
    this->player = player;

    this->num_moves = this->board.find_num_moves(this->player);
    this->children = std::vector<MoveTree*>();
    this->child_indices = std::unordered_set<int>();

    this->num_wins = 0;
    this->num_rollouts = 0;
}

/*
 * This is the constructor used within the class to create children
 *
 * Params:
 *     parent - a pointer to the parent node
 *     move - the move which transposes the parent's board to this one's
 *     index - the index of this tree in the parent's chilren array
 */
MoveTree::MoveTree(MoveTree *parent, move_t move) {
    this->parent = parent;
    this->prev_move = move;

    this->board = parent->board.make_move_immutably(parent->player, move);
    this->player = !(parent->player);

    this->num_moves = this->board.find_num_moves(this->player);
    this->children = std::vector<MoveTree*>();
    this->child_indices = std::unordered_set<int>();
    
    this->num_wins = 0;
    this->num_rollouts = 0;
}

/*
 * MoveTree destructor
 * Recursively destroys children too
 */
MoveTree::~MoveTree() {
    for(MoveTree *child : children) {
        delete child;
    }
}

/*
 * Updates this node's counts according to the result of this rollout
 *
 * Params:
 *     eval - the evaluation of the final position of a rollout
 * Return: none
 */
inline void MoveTree::update_counters(int eval) {
    this->num_rollouts++;
    if(first_better(this->player, 0, eval))  { 
        // if the player who moved to this position won (aka position is bad for current player)
        this->num_wins++;
    }
}

/*
 * Calculates how favorable it is to continue to this node during a rollout
 * Favorability, or "promise", is a combination of how strong the move to
 * this position is for the mover to it, and how unexplored this node is
 *
 * Params: none
 * Return: a float - the higher, the more promising this node is
 */
inline float MoveTree::promise() {
    float exploration = 1 / (log(this->num_rollouts + 1) + 1);
    float exploitation = 1/2; // default for no rollouts
    if(this->num_rollouts > 0) { // we can divide by num_rollouts
        exploitation = (float)this->num_wins / this->num_rollouts;
    }
    return exploration + exploitation;
}

/*
 * Returns the index in the children list of the node which should continue 
 * the rollout, as calculated by the promise() method
 * If there's a tie, randomly selects one of the tied best children
 *
 * Params: none
 * Return: an int - the index of the most lucrative continuation
 */
int MoveTree::most_promising_index(float floor) {
    std::vector<int> best_indices;
    float greatest_promise = floor;
    float current_promise;

    for(int i=0; i < (int)this->children.size(); i++) {
        current_promise = this->children[i]->promise();
        if(current_promise > greatest_promise) {
            greatest_promise = current_promise;
            best_indices.clear();
            best_indices.push_back(i);
        }
        if(current_promise == greatest_promise) {
            best_indices.push_back(i);
        }
    }
    if(best_indices.empty())
        return -1;
    return best_indices[rand() % best_indices.size()];
}

/*
 * Constructs a child node, randomly selected from the moves from this position
 * for which we haven't yet constructed a child. Also adds the index in of it
 * (in the list returned from this->board.get_moves()) to the child_indices set
 *
 * Params: none
 * Return: none
 */
void MoveTree::open_new_node() {
    int num_options = this->num_moves - this->children.size();
    assert(num_options > 0);
    int random_index = rand() % num_options;

    std::vector<move_t> moves = this->board.get_moves(this->player);
    int j=0;
    for(int i=0; i < (int)moves.size(); i++) {
        if(this->child_indices.count(i)) continue;
        if(j == random_index) {
            this->children.push_back(new MoveTree(this, moves[i]));
            this->child_indices.insert(i);
            break;
        }
        j++;
    }
}

/*
 * Simulates a semirandom continuation of the game. Updates the counters
 * of the positions reached in this simulation based on whether the end 
 * result was good or bad for the player whose turn it was in that position
 *
 * Params:
 *     depth - the number of moves to simulate before evaluating the position
 * Return: an int - the evaluation of the final position of the simulation
 */
int MoveTree::rollout(int depth) {
    int continuation_index;
    int eval;

    if(depth == 0) {
        eval = this->board.evaluate();
        this->update_counters(eval);
        return eval;
    }
    if(this->num_moves == 0) { // no moves; this->player loses
        update_counters(worst_eval(this->player));
        return worst_eval(this->player);
    }

    // if there are moves that we haven't considered, include them in the search by 
    // starting with 1.5 promise (the promise of a node with no rollouts)
    continuation_index = this->most_promising_index(num_moves == (int)children.size() ? 0 : 1.5);
    if(continuation_index == -1) { // we should explore a new node
        this->open_new_node(); // will push new node to back of children list
        assert(!this->children.empty());
        eval = this->children.back()->rollout(depth - 1); // children.back() is new node
    } else { // use the node we just got an index for
        eval = this->children[continuation_index]->rollout(depth - 1);
    }

    this->update_counters(eval);
    return eval;
}

/*
 * Finds the best move in the position based on the results of MCTS
 *
 * Params: none
 * Returns: an int - the index of the best move in the children list
 */
int MoveTree::best_move_index() {
    int most_wins = 0;
    int index = 0;

    for(uint i=0; i < this->children.size(); i++) {
        if(this->children[i]->num_wins > most_wins) {
            most_wins = this->children[i]->num_wins;
            index = i;
        }
    }
    return index;
}

/*
 * The AI makes a move on board. It then trims the move tree to only contain
 * nodes decending from the new position (the rest of the tree is freed)
 * Usage: tree.make_move_and_die(board, &tree);
 *
 * Params: 
 *     board - the main game board on which the AI will move
 * Returns: none - the object calling this method deletes itself, so it can't return
 */
move_t MoveTree::make_move(Board& board) {
    // do MCTS
    this->think();

    // make the move
    int child_index = this->best_move_index();
    assert(board.make_move(this->player, this->children[child_index]->prev_move));

    return this->children[child_index]->prev_move;
}

// returns whether two moves are the same (insofar as Board::make_move() is concerned)
bool moves_equal(move_t move_1, move_t move_2) {
    if(!move_1.old_loc.equals(move_2.old_loc)) return false;
    if(!move_1.new_loc.equals(move_2.new_loc)) return false;
    if(!move_1.arrow.equals(move_2.arrow)) return false;
    return true;
}

/*
 * A few seconds are spent evaluating the moves using MCTS
 *
 * Params: none
 * Return: none
 */
void MoveTree::think() {
    printf("The computer is thinking...\n");

    for(int i=0; i < ROLLOUTS; i++) {
        this->rollout(SEARCH_DEPTH);
    }
}

