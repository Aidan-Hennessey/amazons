#include <stdio.h>
#include <assert.h>
#include <chrono>
#include <iostream>
#include "amazons.hpp"
#include "Board.hpp"
#include "UI.hpp"
#include "ai.hpp"
#include "MoveTree.hpp"

using namespace std;

/*
 * Removes an element from a vector in constant time, but without maintaining 
 * ordering
 *
 * Params:
 *     vector - the vector from which an element will be removed
 *     index - the index of the element to remove
 * Return: none
 */
template <typename T>
void constant_remove(std::vector<T>& vector, int index) {
    assert(vector.size() > 0);
    vector[index] = vector[vector.size() - 1];
    vector.erase(vector.end() - 1);
}

int main() {
    printf("%i", sizeof(MoveTree));

    return 0;
}