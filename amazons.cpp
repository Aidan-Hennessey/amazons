#include <stdio.h>
#include <stdlib.h>
#include "amazons.hpp"
#include "Board.hpp"
#include "UI.hpp"

int main() {
    char action;

    while(true) {
        action = start_screen();

        switch(action) {
            case 'r':
                printf("soon the rules will go here\n\n\n");
                break;
            case 't':
                play_game(false, false);
                break;
            default:
                exit_app();
        }
    }

    return 0;
}

/*
 * Gets and makes moves from each player until someone can't go
 *
 * Params:
 *     left_ai - whether the left player is an AI
 *     right_ai - whether the right player is an AI
 */
void play_game(bool left_ai, bool right_ai) {
    Board board;
    player_t current_player = left; //left goes first

    while(!board.no_moves(current_player)) {
        if((current_player == left) ? left_ai : right_ai) { // if it's an ai's turn
            //TODO
        } else { // it's a human's turn
            human_move(board, current_player);
        }
        // swap current player
        current_player = (current_player == left) ? right : left;
    }

    game_over(current_player);
}