// This is a header file with some micscellaneous types
// that are used throughout the project

#include <stdlib.h>

typedef enum {left, right} player_t;

typedef enum {lamazon, ramazon, burnt, open} tile_state_t;

/*
 * The coordinate of a tile in the game
 */
class Point {
    int row; // 1 indexed, top to bottom
    int col; // 1 indexed, left to right

    public:
    Point() {
        row = 1;
        col = 1;
    }

    Point(int row, int col) {
        this->row = row;
        this->col = col;
    }

    // calculates the coordinates associated with a bitboard index
    Point(int bb_index) {
        row = bb_index / 12;
        col = bb_index % 12;
    }

    int get_row() {return row;}
    int get_col() {return col;}
    void set_row(int new_row) {row = new_row;}
    void set_col(int new_col) {col = new_col;}
    void incr_row() {row++;}
    void incr_col() {col++;}

    static inline int rowcol_to_val(int row, int col) {
        return 12 * row + col;
    }

    // calculates the index of the bitboard bit associated with the tile 
    // represented by this point
    int to_bbval() {
        return rowcol_to_val(row, col);
    }
};

/*
 * Gets and makes moves from each player until someone can't go
 *
 * Params:
 *     left_ai - whether the left player is an AI
 *     right_ai - whether the right player is an AI
 */
void play_game(bool left_ai, bool right_ai);