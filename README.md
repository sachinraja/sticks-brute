# sticks-brute

This is a algorithm brute forcing the sticks game to find out if it is "solved" for the first player, meaning the first player can win no matter what the second player plays.

It works by generating the possible moves from a given position: each hand that can be hit by the player whose turn it is (from either of their own hands) and the possible splits the player can make.

Given this set of possible moves it enforces that:
1. If it is the first player's turn, there must be one move that leads to the first player winning. 
2. If it is the second player's turn, all moves must have possible wins for the first player.

Finally the base case: if either player's hand reaches 0 fingers, they lose and the other player wins.

This algorithm determines that, yes, the game is solved for the first player.

## Ideas
Make it configurable so I can ask the program if it is solved for either player, instead of it always checking if it solved for the first player.
