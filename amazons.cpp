#include <stdio.h>
#include <stdlib.h>
#include "amazons.hpp"
#include "Board.hpp"
#include "UI.hpp"
#include "ai.hpp"

#ifndef TESTS

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
            case 'f':
                play_game(false, true);
                break;
            case 's':
                play_game(true, false);
                break;
            case 'w':
                play_game(true, true);
                break;
            default:
                exit_app();
        }
    }

    return 0;
}

#endif

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
    move_t bot_move;

    while(!board.no_moves(current_player)) {
        board.evaluate_verbose();
        board.print();

        if((current_player == left) ? left_ai : right_ai) { // if it's an ai's turn
            bot_move = ai_move(board, current_player);
            bot_move_recognition(board, bot_move);
        } else { // it's a human's turn
            human_move(board, current_player);
        }
        // swap current player
        current_player = (current_player == left) ? right : left;
    }

    game_over(current_player);
}