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
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    timeout(0);

    char c;

    // print title screen
    printf("      __ _  _  _   __ _ ____  ___  _ ___  ___| |\n");
    printf("     / _` |/ \\/ \\ / _` |\\  _|/ _ \\| `_  \\/ __| |\n");
    printf("    | (_| | |\\/| | (_| |_\\ \\| (_) | | | |__  |_|\n");
    printf("     \\__,_|_|  |_|\\__,_|____\\___/|_| | |___/(_)\n\n");
    printf("Welcome to the game of amazons\n");
    printf("To read the rules of the game, press 'r'\n");
    printf("To play a two player game, press 't'\n");
    printf("To play moving first against an AI, press 'f'\n");
    printf("To play moving second against an AI, press 's'\n");
    printf("To exit the app, press any other key\n");

    // get char
    while(true) {
        c = getch();
        if(c != ERR)
            return c;
    }
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

    printf("   1 2 3 4 5 6 7 8 9 10\n");
    printf("  +-+-+-+-+-+-+-+-+-+-+\n"); //top of board
    for(int i=1; i <= 10; i++) {
        printf("%2i|", i);
        for(int j=1; j <= 10; j++) {
            printf("%c|", board[i][j]);
        }
        printf("\n  +-+-+-+-+-+-+-+-+-+-+\n");
    }
}

// Exits the app from the title screen
void exit_app() {
    printf("Thanks for playing!\n");
    exit(0);
}

// Prints a message declaring the victor
void game_over(player_t loser) {
    if(loser == left) {
        printf("Player 2 won!\n\n\n");
    } else {
        printf("Player 1 won!\n\n\n");
    }
}

/*
 * Tokenizes the buffer, filling the tokens array with pointers to the starts
 * of the tokens. Returns the number of tokens
 *
 * Params: 
 *     buffer: a char array representing user input
 *     tokens: the tokenized input
 * Return:
 *     an int - the number of tokens found by the parser
 */
int parse(char buffer[BUFLEN], char *tokens[BUFLEN / 2]) {
    // TODO: write your code here
    int location = 0;
    int size;
    int token_index = 0;
    
    while(1) {
        location += strspn(&buffer[location], "\t\n ,");

        if(buffer[location] == '\0') break;
        else {
            size = strcspn(&buffer[location], " \t\n,");
            buffer[location + size] = '\0';
            tokens[token_index++] = &buffer[location];
            location += size + 1;
        }
    }
    return token_index;
}

/*
 * Helper for human_move()
 * Takes tokenized input from the user and fills a point object from it
 * Returns 0 on success, -1 on error
 *
 * Params:
 *     tokens - the two tokens returned from the parser
 *     location - the point object in which we will store the user input
 */
int point_from_user_input(char *tokens[], Point& location) {
    int row = atoi(tokens[0]);
    int col = atoi(tokens[1]);

    if(row > 10 || row < 1) return -1;
    if(col > 10 || col < 1) return -1;

    location.set_row(row);
    location.set_col(col);

    return 0;
}

void human_move(Board board, player_t current_player) {
    char buf[BUFLEN] = {};
    char *tokens[BUFLEN/2];
    int num_tokens;
    Point old_loc;
    Point new_loc;
    Point arrow;

    while(true) {
        // select the amazon to move
        printf("Please enter the coordinates of the amazon you would like to move.\n");
        printf("Format: row #, comma, space, row #. Example: '6, 7'.\n");
        printf("Your move: ");
        fflush(stdout);
        fgets(buf, BUFLEN - 1, stdin);

        num_tokens = parse(buf, tokens);
        if(num_tokens != 2) {
            printf("Invalid input. Please try again.\n");
            continue;
        }

        if(point_from_user_input(tokens, old_loc)) { 
            // if the function fails
            printf("Invalid input. Please try again.\n");
            continue;
        }

        // select the amazon's new location
        printf("now enter the coordinates you would like her to move to: ");
        fflush(stdout);
        fgets(buf, BUFLEN - 1, stdin);

        num_tokens = parse(buf, tokens);
        if(num_tokens != 2) {
            printf("Invalid input. Please try again.\n");
            continue;
        }

        if(point_from_user_input(tokens, new_loc)) { 
            // if the function fails
            printf("Invalid input. Please try again.\n");
            continue;
        }

        // select the arrow's location
        printf("finally, enter the coordinates of the square you would like to burn: ");
        fflush(stdout);
        fgets(buf, BUFLEN - 1, stdin);

        num_tokens = parse(buf, tokens);
        if(num_tokens != 2) {
            printf("Invalid input. Please try again.\n");
            continue;
        }

        if(point_from_user_input(tokens, arrow)) { 
            // if the function fails
            printf("Invalid input. Please try again.\n");
            continue;
        }

        if(board.make_move(current_player, old_loc, new_loc, arrow))
            break;
        else {
            printf("That is not a legal move. Please try again.\n");
            continue;
        }
    }
}