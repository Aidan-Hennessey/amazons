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

// Prints a message declaring the victor
void game_over(player_t loser);

void human_move(Board board, player_t current_player);