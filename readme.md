# Briwats Chess Engine

BriwatsCE is a UCI chess engine rated at 2014 ELO, the rating was determined by using cutechess to play BriwatsCE against multiple version of [Stash chess engine](https://www.chessprogramming.org/Stash).

It uses 0x88 board representation and can either use MTD(f) or normal AlphaBetaWithMemory (aka alpha beta with transposition table) for the search, i implemented chess from scratch which was a pain to implement!

Currently as of version 0.1 the NNUE is trained but not currently plugged to the engine, so playing against this version is playing against a hand-made evaluation function instead of a NNUE one! i'll probably re-train the MLP to consider multiple other features too (which side to play, current ply...)

## How to play:

Make sure you have a [lichess](lichess.org) account, and challenge the BOT via this link:

[https://lichess.org/@/Briwats](https://lichess.org/@/Briwats)

---

### Features:

- Minimax and Quiescence search
- UCI integration
- Time management + Iterative deepening
- A FEN parser
- Transposition table
- Implemented [MTD(f)](https://people.csail.mit.edu/plaat/mtdf.html)
- Implemented killer heuristic
- Implemented history heuristic
- Implemented and trained a MLP on [Lichess elite database](https://database.nikonoel.fr/)
- Move generation (working chess)
- Basic evaluation function
- Basic NNUE

<!-- ### TODO:

- [ ] Make sure the nnue works (use some basic FENs)
- [ ] Write the article about NNUE
- [ ] Use the nnue :) (shockers)
- [ ] Output bucket nnue -->