#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "amazons.hpp"
#include "Board.hpp"
#include "UI.hpp"
#include "ai.hpp"
#include "MoveTree.hpp"

bool times_up;
pthread_rwlock_t times_up_lock = PTHREAD_RWLOCK_INITIALIZER;

int memory_allocated = 0;

#ifndef TESTS

int main(int argc, char *argv[]) {
    srand(time(NULL));

    char action;
    bool print_eval = false;

    if(argc > 1 && strcmp(argv[1], "--v") == 0)
        print_eval = true;

    while(true) {
        action = start_screen();

        switch(action) {
            case 'r':
                print_rules();
                break;
            case 't':
                play_game(false, false, print_eval);
                break;
            case 'f':
                play_game(false, true, print_eval);
                break;
            case 's':
                play_game(true, false, print_eval);
                break;
            case 'w':
                play_game(true, true, print_eval);
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
 *     print_eval - whether to print an AI evaluation of the position
 * Return: none
 */
void play_game(bool left_ai, bool right_ai, bool print_eval) {
    Board board;
    player_t current_player = LEFT; //left goes first

    while(!board.no_moves(current_player)) {
        if(print_eval)
            board.evaluate_verbose();

        if((current_player == LEFT) ? left_ai : right_ai) { // if it's an ai's turn
            // bot_move = ai_move(board, current_player);
            board.print();
            bot_move_recognition(board, ai_move(board, current_player));
        } else { // it's a human's turn
            board.print();
            human_move(board, current_player);
        }
        // swap current player
        current_player = !current_player;
    }

    game_over(current_player);
}