#ifndef MOVETREE_H
#define MOVETREE_H

#include <stdlib.h>
#include <vector>
#include <unordered_set>
#include "amazons.hpp"
#include "Board.hpp"

#define ROLLOUTS 10000
#define SEARCH_DEPTH 20

class MoveTree {
    MoveTree *parent;
    move_t prev_move;

    Board board;
    player_t player;

    std::vector<MoveTree *> children;
    int num_moves; // the number of legal moves from the position, not the size of the vector
    std::unordered_set<int> child_indices; // the indices of the elts of this->children in this->board.get_moves()

    int num_wins;
    int num_rollouts;

    public:
    /*
     * This is the constructor used from outside the class to construct an empty tree
     *
     * Params:
     *     board - the board state of the tree
     *     player - whose turn it is to move
     */
    MoveTree(Board board, player_t player);

    /*
     * This is the constructor used within the class to create children
     *
     * Params:
     *     parent - a pointer to the parent node
     *     move - the move which transposes the parent's board to this one's
     *     index - the index of this tree in the parent's chilren array
     */
    MoveTree(MoveTree *parent, move_t move);

    /*
     * MoveTree destructor
     * Recursively destroys children too
     */
    ~MoveTree();

    move_t get_prev_move() {return this->prev_move;}

    /*
     * Updates this node's counts according to the result of this rollout
     *
     * Params:
     *     eval - the evaluation of the final position of a rollout
     * Return: none
     */
    inline void update_counters(int eval);

    /*
     * Calculates how favorable it is to continue to this node during a rollout
     * Favorability, or "promise", is a combination of how strong the move to
     * this position is, and how unexplored this node is
     *
     * Params: none
     * Return: a float - the higher, the more promising this node is
     */
    inline float promise();

    /*
     * Returns the index in the children list of the node which should continue 
     * the rollout, as calculated by the promise() method
     * If there's a tie, randomly selects one of the tied best children
     *
     * Params: none
     * Return: an int - the index of the most lucrative continuation
     */
    int most_promising_index(float floor);

    /*
     * Constructs a child node, randomly selected from the moves from this position
     * for which we haven't yet constructed a child. Also adds the index in of it
     * (in the list returned from this->board.get_moves()) to the child_indices set
     *
     * Params: none
     * Return: none
     */
    void open_new_node();

    /*
     * Simulates a semirandom continuation of the game. Updates the counters
     * of the positions reached in this simulation based on whether the end 
     * result was good or bad for the player whose turn it was in that position
     *
     * Params:
     *     depth - the number of moves to simulate before evaluating the position
     * Return: an int - the evaluation of the final position of the simulation
     */
    int rollout(int depth);

    /*
     * Finds the best move in the position based on the results of MCTS
     *
     * Params: none
     * Returns: an int - the index of the best move in the children list
     */
    int best_move_index();

    /*
     * A few seconds are spent evaluating the moves using MCTS
     *
     * Params: none
     * Return: none
     */
    void think();

    move_t make_move(Board& board);
};

#endif