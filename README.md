# Amazons

This program allows you to play the Game of the Amazons board game in terminal against a friend or an AI.

[The Game of the Amazons](https://en.wikipedia.org/wiki/Game_of_the_Amazons) is a board game invented in 1988 by Walter Zamkauskas. I first learned about it when I saw it featured in _Winning Ways for Your Mathematical Plays_, a book on combinatorial game theory.

# Compilation

This program is written for linux; portability to other operating systems has not been tested.

To compile, run the command "make amazons" in the command line. This will generate the amazons executable.

# Running

To run, simply run ./amazons in the command line. This will bring you to the title screen, from which point you can decide what you would like to do.

# Potential Improvements

Writing a strong AI for this game is challenging for two main reasons:
1. Unlike chess or go, there is no established professional scene from which we can borrow expert knowledge to craft good heuristics.
2. The branching factor for this game is enormous (consistently over 1000 in the early game) so naive minimax approaches are intractible at any high depth.

The current implemenation of the AI finds every possible move it can make, and evaluates the positions resulting from each move by a simple heuristic. It then makes the move which results in the position with the most favorable evaluation. The heuristic is a linear combination of the difference in number of legal moves available to the AI vs its opponent, and the difference in number of reachable squares between the AI and its opponent.

As you can imagine, this AI is not strong; it has no foresight or capacity for long term strategy.

To improve the AI, I could do one or many of the following:
1. Add minimax so the AI searches at greater depth (could probably only get to depth ~2 but that'd still be an improvement)
2. Implement Monte Carlo tree search. Richard J. Lorentz and colleagues found success combining MCTC with heuristic evaluation in their 2008 paper, which you can read [here](https://link.springer.com/chapter/10.1007/978-3-540-87608-3_2).
3. Refine the hueristic using a genetic algorithm. One idea that could prove particularly fruitful given my lack of game knowledge would be to implement a genetic algorithm to refine my AI's hueristic. If I were to this, I would give it more parameters to play with, like sum of squares of number of moves from each amazon, squares easier to reach for the AI vs the opponent, etc.
4. Replace the heuristic with a neural network. Several people have emulated the success of the deep learning techniques which made Google's AlphaZero and AlphaGo programs so successful at chess and go for Amazons, creating very strong AIs. You can read about one example [here](https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=8408297).
