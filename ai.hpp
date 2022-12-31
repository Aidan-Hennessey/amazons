#ifndef AI_H
#define AI_H

#include "Board.hpp"
#include "MoveTree.hpp"

#define ALPHA 100 // empirically chosen parameter
#define BIGNUM 999999 //greater than any possible evaluation value
#define worst_eval(p) (p ? -BIGNUM : BIGNUM)
#define first_better(p, a, b) (p ? a > b : a < b)

#define SLEEP_TIME 1 // the number of seconds the AI spends per move
#define ROLLOUTS 10000
#define SEARCH_DEPTH 20

/*
 * The ai makes a move
 *
 * Params:
 *     player - which player the ai is moving for
 *     board - the board on which to make a move
 * Return: none
 */
move_t ai_move(Board& board, player_t player);

/*
 * Runs MCTS on a tree until it recieves a signal to terminate
 *
 * Params:
 *     args - a pointer to the tree that should call rollout()
 * Return: none
 */
void *search_move(void *args);

#endif