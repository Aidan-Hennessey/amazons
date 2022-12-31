# Amazons

This program allows you to play the Game of the Amazons board game in terminal against a friend or an AI.

[The Game of the Amazons](https://en.wikipedia.org/wiki/Game_of_the_Amazons) is a board game invented in 1988 by Walter Zamkauskas. I first learned about it when I saw it featured in _Winning Ways for Your Mathematical Plays_, a book on combinatorial game theory.

# Compilation

This program is written for linux; portability to other operating systems has not been tested.

To compile, run the command "make amazons" in the command line. This will generate the amazons executable. You can also run "make small_amazons" or "make tiny_amazons". These generate executables that allow you to play on 8x8 and 6x6 boards respectively.

# Running

To run, simply run ./amazons in the command line. This will bring you to the title screen, from which point you can decide what you would like to do. You can also include the flag --verbose to make the program print a heuristic evaluation of the position after each move.

NOTE: The program looks best when your terminal window displays 30 lines at a time. (26 and 22 for small and tiny)

# Potential Improvements

Writing a strong AI for this game is challenging for two main reasons:
1. Unlike chess or go, there is no established professional scene from which we can borrow expert knowledge to craft good heuristics.
2. The branching factor for this game is enormous (consistently over 1000 in the early game) so naive minimax approaches are intractible at any high depth.

The current implementation of the AI uses [Monte Carlo tree search](https://en.wikipedia.org/wiki/Monte_Carlo_tree_search) to simulate 20 moves, and then uses a simple heuristic to evaluate the resulting posistion. The heuristic is a linear combination of the difference in number of legal moves available to the AI vs its opponent, and the difference in number of reachable squares between the AI and its opponent.

It's hard for me to gauge the strength of my AI. It can reliably beat me, but that doesn't say very much.

To improve the AI, I could do one or many of the following:
1. Multithreading - I could speed up my AI's search of the move tree with concurrency. This would be a little tricky, as a naive fine-grained locking approach would cause deadlock because my rollout algorithm traverses down the tree and then back up it to update the counts for each node. A thread trying to go down an edge that another thread is trying to go up would cause deadlock. There are ways around this, however.
2. Refine the hueristic using a genetic algorithm. One idea that could prove particularly fruitful given my lack of game knowledge would be to implement a genetic algorithm to refine my AI's hueristic. If I were to this, I would give it more parameters to play with, like sum of squares of number of moves from each amazon, squares easier to reach for the AI vs the opponent, etc.
3. Replace the heuristic with a neural network. Several people have emulated the success of the deep learning techniques which made Google's AlphaZero and AlphaGo programs so successful at chess and go for Amazons, creating very strong AIs. You can read about one example [here](https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=8408297).
