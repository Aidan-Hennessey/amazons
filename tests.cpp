#include <stdio.h>
#include <assert.h>
#include "amazons.hpp"
#include "Board.hpp"
#include "UI.hpp"
#include "ai.hpp"

int main() {
    Board board;
    printf("should be 27: %i\n", board.num_queen_connections(52));
    assert(board.make_move(left, {Point(1, 4), Point(4, 4), Point(2, 2)}));
    printf("should be 22: %i\n", board.num_queen_connections(52));
}