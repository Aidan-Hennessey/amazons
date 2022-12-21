#ifndef AI_H
#define AI_H

#include "Board.hpp"

#define ALPHA 50 // empirically chosen parameter
#define BIGNUM 999999 //greater than any possible evaluation value
#define worst_eval(p) ((p == left) ? -BIGNUM : BIGNUM)
#define first_better(p, a, b) ((p == left) ? a < b : a > b)

/*
 * The ai makes a move
 *
 * Params:
 *     player - which player the ai is moving for
 *     board - the board on which to make a move
 * Return: none
 */
void ai_move(Board& board, player_t player);

#endif