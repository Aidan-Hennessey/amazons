#ifndef AMAZONS_H
#define AMAZONS_H

// This is a header file with some micscellaneous types
// that are used throughout the project

#include <stdlib.h>

#ifdef TINY
  #define BOARDWIDTH 6
#else
  #ifdef SMALL
    #define BOARDWIDTH 8
  #else
    #define BOARDWIDTH 10
  #endif
#endif

#define BBWIDTH (BOARDWIDTH + 2)
#define SETSIZE (BBWIDTH * BBWIDTH)
#define INCRS_INIT {-BBWIDTH - 1, -BBWIDTH, -BBWIDTH + 1, \
                    -1, 1, \
                    BBWIDTH - 1, BBWIDTH, BBWIDTH + 1}
#define TOP_LEFT (BBWIDTH + 1)
#define BOTTOM_RIGHT (BBWIDTH * (BBWIDTH - 1) - 2)

typedef bool player_t;

#define LEFT true
#define RIGHT false

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
        row = bb_index / BBWIDTH;
        col = bb_index % BBWIDTH;
    }

    int get_row() const {return row;}
    int get_col() const {return col;}
    void set_row(int new_row) {row = new_row;}
    void set_col(int new_col) {col = new_col;}
    void incr_row() {row++;}
    void incr_col() {col++;}

    bool equals(Point other) const {return row == other.get_row() && col == other.get_col();}

    static inline int rowcol_to_val(int row, int col) {return BBWIDTH * row + col;}

    // calculates the index of the bitboard bit associated with the tile 
    // represented by this point
    int to_bbval() const {return rowcol_to_val(row, col);}
};

typedef struct move {
    Point old_loc;
    Point new_loc;
    Point arrow;
} move_t;

/*
 * Gets and makes moves from each player until someone can't go
 *
 * Params:
 *     left_ai - whether the left player is an AI
 *     right_ai - whether the right player is an AI
 *     print_eval - whether to print an AI evaluation of the position
 * Return: none
 */
void play_game(bool left_ai, bool right_ai, bool print_eval);

#endif