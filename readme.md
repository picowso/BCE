# Briwats Chess Engine

BriwatsCE is a UCI chess engine rated at 2014ELO, the rating was determined by using cutechess to play BriwatsCE against multiple version of [Stash chess engine](https://www.chessprogramming.org/Stash).

It uses 0x88 board representation and can either use MTD(f) or normal AlphaBetaWithMemory (aka alpha beta with transposition table) for the search, i implemented chess from scratch which was a pain to implement!

Currently as of version 0.1 the NNUE is trained but not currently plugged to the engine, so playing against this version is playing against a hand-made evaluation function instead of a NNUE one! i'll probably re-train the MLP to consider multiple other features too (which side to play, current ply...)

---

### TODO:

- [X] Move generation
- [X] Basic evaluation function
- [X] Minimax and Quiescence search
- [X] UCI integration
- [X] Time management + Iterative deepening
- [X] A FEN parser
- [X] Transposition table
- [X] [MTD(f)](https://people.csail.mit.edu/plaat/mtdf.html) implementation
- [X] Implement killer heu
- [X] Implement history heu
- [X] Implement and train a MLP on [Lichess elite database](https://database.nikonoel.fr/) 
- [ ] Implement NNUE