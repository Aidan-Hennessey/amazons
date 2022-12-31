#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
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
    printf("\n\n\n\n\n      __ _  _  _   __ _ ____  ___  _ ___  ___| |\n");
    printf("     / _` |/ \\/ \\ / _` |\\  _|/ _ \\| `_  \\/ __| |\n");
    printf("    | (_| | |\\/| | (_| |_\\ \\| (_) | | | |__  |_|\n");
    printf("     \\__,_|_|  |_|\\__,_|____\\\\___/|_| | |___/(_)\n\n\n\n\n\n");
    printf("Welcome to the game of amazons\n\n");
    printf("To read the rules of the game, press 'r'\n");
    printf("To play a two player game, press 't'\n");
    printf("To play moving first against an AI, press 'f'\n");
    printf("To play moving second against an AI, press 's'\n");
    printf("To watch the AI play against itself, press 'w'\n");
    printf("To exit the app, press any other key\n\n");

    // get char
    char buf[BUFLEN];
    fgets(buf, BUFLEN - 1, stdin);
    return buf[0];
}

void print_boarder_line() {
    printf("  +");
    for(int i=0; i < BOARDWIDTH; i++) {
        printf("---+");
    }
    printf("\n");
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
void print_board(char board[BOARDWIDTH][BOARDWIDTH]) {
    printf("key:\n");
    printf("%c - player 1 amazon\n", tile_icon(lamazon));
    printf("%c - player 2 amazon\n", tile_icon(ramazon));
    printf("%c - burnt square\n\n", tile_icon(burnt));

    //printf("    A   B   C   D   E   F   G   H   I   J\n");
    printf(" ");
    for(int i=0; i < BOARDWIDTH; i++) {
        printf("   %c", 'A' + (char)i);
    }
    printf("\n");
    //printf("  +---+---+---+---+---+---+---+---+---+---+\n"); //top of board
    print_boarder_line();

    for(int i=0; i < BOARDWIDTH; i++) {
        printf("%2i|", BOARDWIDTH - i);
        for(int j=0; j < BOARDWIDTH; j++) {
            printf(" %c |", board[i][j]);
        }
        //printf("\n  +---+---+---+---+---+---+---+---+---+---+\n");
        printf("\n");
        print_boarder_line();
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
    char buf[BUFLEN];

    if(loser == LEFT) { //player 2 won
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

    printf("Press enter to return to the home screen\n\n\n");
    read(0, buf, BUFLEN);
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

    row = BOARDWIDTH + 1 - atoi(&token[1]); // flipping from 1=bottom to 1=top

    if(row < 1 || row > BOARDWIDTH) return -1;
    if(col < 1 || col > BOARDWIDTH) return -1;

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
move_t human_move(Board& board, player_t current_player) {
    char buf[BUFLEN] = {};
    char *tokens[BUFLEN/2];
    int num_tokens;
    move_t move;

    while(true) {
        // get move from user
        printf("Player %i, please enter your move.\n", (current_player == LEFT) ? 1 : 2);
        printf("Format: start - end (arrow). Example: 'A4 - A7 (C5)'.\n");
        printf("Your move: ");
        fflush(stdout);
        fgets(buf, BUFLEN - 1, stdin);

        // parse input
        num_tokens = parse(buf, tokens);
        if(num_tokens != 3) {
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
            return move;
        else {
            printf("That is not a legal move. Please try again.\n");
            continue;
        }
    }
}

// converts the column number to its corresponding letter (1->A, 2->B, etc)
char colnum_to_letter(int colnum) {
    return (char)(colnum + 0x40);
}

/*
 * Prints a neatly formmated move to the screen
 *
 * Params:
 *     move - the move to print
 * Returns: none - just prints to stdout
 */
void print_move(move_t move) {
    char old_col = colnum_to_letter(move.old_loc.get_col());
    int old_row = BOARDWIDTH + 1 - move.old_loc.get_row();
    char new_col = colnum_to_letter(move.new_loc.get_col());
    int new_row = BOARDWIDTH + 1 - move.new_loc.get_row();
    char arrow_col = colnum_to_letter(move.arrow.get_col());
    int arrow_row = BOARDWIDTH + 1 - move.arrow.get_row();

    printf("%c%i - %c%i (%c%i)\n", old_col, old_row, new_col, new_row, arrow_col, arrow_row);
}

/*
 * Prompts the user to recognize that the AI has made a move
 *
 * Params:
 *     board - the game board now that the AI has made a move
 *     move - the move the AI made
 * Return: none
 */
void bot_move_recognition(Board board, move_t move) {
    char *buf[BUFLEN];
    board.print();
    printf("The AI made the following move:\n");
    print_move(move);
    printf("Press enter to continue.");
    fflush(stdout);
    read(0, buf, BUFLEN);
}

// prints the rules of the game
void print_rules() {
    printf("\n\n");

    char ch;
    FILE *rules = fopen("rules.txt", "r");
 
    if (rules == NULL) {
        printf("rules file can't be opened :(\n\n");
    }
 
    ch = fgetc(rules);
    while (ch != EOF) {
        printf("%c", ch);
        ch = fgetc(rules);
    }
 
    fclose(rules);

    char buf[BUFLEN];
    printf("\n\nTo return to the home screen, press enter.\n");
    read(0, buf, BUFLEN);
}