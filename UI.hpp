#ifndef UI_H
#define UI_H

#include "Board.hpp"
#include "amazons.hpp"

// converts from the enum representing a square's state to the icon printed to 
// indicate that state
#define tile_icon(t) (t == open) ? ' ' : \
                    (t == burnt) ? 'X' : \
                    (t == lamazon) ? '@' : 'O'

/*
 * Prints the title screen and waits for the user to type a character
 *
 *      __ _  _  _   __ _ ____  ___  _ ___  ___| |
 *     / _` |/ \/ \ / _` |\  _|/ _ \| `_  \/ __| |
 *    | (_| | |\/| | (_| |_\ \| (_) | | | |__  |_|
 *     \__,_|_|  |_|\__,_|____\\___/|_| | |___/(_)
 *
 * Params: None
 * Returns: the character entered by the user
 */
char start_screen();

/*
 * Prints the board to the screen
 *
 * Params:
 *     board - a 2d array of the characters that should be printed in each
 *             square of the game board
 * Returns:
 *     None
 */
void print_board(char board[10][10]);

// Exits the app from the title screen
void exit_app();

/*
 * Prints a message declaring the victor
 *                                        _
 *   ___ | | __ _ __    __ ___  _ __    /_ |
 *  /   \| |/ _` |\ \  / // _ \| `__|    | |
 * | (_) | | (_| | \ \/ /|  __|| |      _| |_
 * | ,__/|_|\__,_|  \  /  \___||_|     |_____|
 * | |              / /
 *  _  _  _  ___  _ __ | |
 * | |/ \| |/ _ \| `_ \| |
 * | | | | | (_) | | | |_|
 *  \_/ \_/ \___/|_| |_(_)
 *
 * Params:
 *     loser - the player who lost the game
 * Return: none
 */
void game_over(player_t loser);

/*
 * The routine for a human move. Gets moves from the user until one is valid 
 * and then makes that move
 *
 * Params:
 *     board - the current board. Used to determine if moves are legal, and
 *             is mutated to make the current move
 *     current_player - whose turn it is (player1/left or player2/right)
 * Return: None
 */
void human_move(Board& board, player_t current_player);

#endif