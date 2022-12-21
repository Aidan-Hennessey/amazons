#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "amazons.hpp"
#include "Board.hpp"
#include "UI.hpp"

#define BUFLEN 32

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
char start_screen() {
    // print title screen
    printf("      __ _  _  _   __ _ ____  ___  _ ___  ___| |\n");
    printf("     / _` |/ \\/ \\ / _` |\\  _|/ _ \\| `_  \\/ __| |\n");
    printf("    | (_| | |\\/| | (_| |_\\ \\| (_) | | | |__  |_|\n");
    printf("     \\__,_|_|  |_|\\__,_|____\\\\___/|_| | |___/(_)\n\n");
    printf("Welcome to the game of amazons\n");
    printf("To read the rules of the game, press 'r'\n");
    printf("To play a two player game, press 't'\n");
    printf("To play moving first against an AI, press 'f'\n");
    printf("To play moving second against an AI, press 's'\n");
    printf("To exit the app, press any other key\n");

    // get char
    char buf[BUFLEN];
    fgets(buf, BUFLEN - 1, stdin);
    return buf[0];
}

/*
 * Prints the board to the screen
 *
 * Params:
 *     board - a 2d array of the characters that should be printed in each
 *             square of the game board
 * Returns:
 *     None
 */
void print_board(char board[10][10]) {
    printf("key:\n");
    printf("%c - player 1 amazon\n", tile_icon(lamazon));
    printf("%c - player 2 amazon\n", tile_icon(ramazon));
    printf("%c - burnt square\n\n", tile_icon(burnt));

    printf("    A   B   C   D   E   F   G   H   I   J\n");
    printf("  +---+---+---+---+---+---+---+---+---+---+\n"); //top of board
    for(int i=1; i <= 10; i++) {
        printf("%2i|", 11 - i);
        for(int j=1; j <= 10; j++) {
            printf(" %c |", board[i-1][j-1]);
        }
        printf("\n  +---+---+---+---+---+---+---+---+---+---+\n");
    }
}

// Exits the app from the title screen
void exit_app() {
    printf("Thanks for playing!\n");
    exit(0);
}

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
void game_over(player_t loser) {
    if(loser == left) { //player 2 won
        printf("\n\n                                       ____\n");
        printf("   ___ | | __ _ __    __ ___  _ __    |__  \\\n");
        printf("  /   \\| |/ _` |\\ \\  / // _ \\| `__|      | /\n");
        printf(" | (_) | | (_| | \\ \\/ /|  __|| |        / /__\n");
        printf(" | ,__/|_|\\__,_|  \\  /  \\___||_|       /____/\n");
        printf(" | |              / /\n");
    } else { // player 1 won
        printf("                                        _\n");
        printf("   ___ | | __ _ __    __ ___  _ __    /_ |\n");
        printf("  /   \\| |/ _` |\\ \\  / // _ \\| `__|    | |\n");
        printf(" | (_) | | (_| | \\ \\/ /|  __|| |      _| |_\n");
        printf(" | ,__/|_|\\__,_|  \\  /  \\___||_|     |_____|\n");
        printf(" | |              / /\n");
    }
    printf("  _  _  _  ___  _ __ | |\n");
    printf(" | |/ \\| |/ _ \\| `_ \\| |\n");
    printf(" | | | | | (_) | | | |_|\n");
    printf("  \\_/ \\_/ \\___/|_| |_(_)\n\n\n");
}

/*
 * Tokenizes the buffer, filling the tokens array with pointers to the starts
 * of the tokens. Returns the number of tokens.
 * With correct input, each token will be a move (letternumber)
 *
 * Params: 
 *     buffer: a char array representing user input
 *     tokens: the tokenized input
 * Return:
 *     an int - the number of tokens found by the parser
 */
int parse(char buffer[BUFLEN], char *tokens[BUFLEN / 2]) {
    int location = 0;
    int size;
    int token_index = 0;
    
    while(1) {
        location += strspn(&buffer[location], "\t\n ,-()");

        if(buffer[location] == '\0') break;
        else {
            size = strcspn(&buffer[location], " \t\n,-()");
            buffer[location + size] = '\0';
            tokens[token_index++] = &buffer[location];
            location += size + 1;
        }
    }
    return token_index;
}

/*
 * Helper for human_move()
 * Takes a move token and fills a point object from it
 * Returns 0 on success, -1 on error
 *
 * Params:
 *     token - the move token to convert
 *     location - the point object in which we will store the square
 */
int move_from_user_input(char *token, Point& location) {
    int row;
    int col;

    if(int(token[0]) > 0x60) { //lowercase letter
        col = int(token[0]) - 0x60; // a->1, b->2, etc
    } else { //uppercase letter
        col = int(token[0]) - 0x40; // A->1, B->2, etc
    }

    row = 11 - atoi(&token[1]); // flipping from 1=bottom to 1=top

    printf("row: %i    col: %i\n", row, col);

    if(row < 1 || row > 10) return -1;
    if(col < 1 || col > 10) return -1;

    location.set_row(row);
    location.set_col(col);

    return 0;
}

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
void human_move(Board& board, player_t current_player) {
    char buf[BUFLEN] = {};
    char *tokens[BUFLEN/2];
    int num_tokens;
    move_t move;

    while(true) {
        // get move from user
        printf("Player %i, please enter your move.\n", (current_player == left) ? 1 : 2);
        printf("Format: start - end (arrow). Example: 'A4 - A7 (C5)'.\n");
        printf("Your move: ");
        fflush(stdout);
        fgets(buf, BUFLEN - 1, stdin);

        // parse input
        num_tokens = parse(buf, tokens);
        if(num_tokens != 3) {
            printf("num_tokens: %i\n", num_tokens);
            printf("Invalid input. Please try again.\n");
            continue;
        }

        // convert input to point objects
        if(move_from_user_input(tokens[0], move.old_loc) || 
            move_from_user_input(tokens[1], move.new_loc) ||
            move_from_user_input(tokens[2], move.arrow)) { 
            // if 1 or more failed
            printf("Invalid input. Please try again.\n");
            continue;
        }

        // make user's move
        if(board.make_move(current_player, move))
            return;
        else {
            printf("That is not a legal move. Please try again.\n");
            continue;
        }
    }
}