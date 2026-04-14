# Briwats Chess Engine

BriwatsCE is a UCI NNUE chess engine, it uses a 0x88 board representation and can either use MTD(f) or normal MiniMax with alpha beta (and TT!) for the search, i implemented chess from scratch which was a pain to code!

The NNUE is trained on the engine's self-play, it's still not stable and can produce catastrophic or random moves in some uncommon positions since it's only trained on ~30 Million games, this number (30M) should increase in future releases!

## How to play:

Make sure you have a [Lichess](https://lichess.org) account, and challenge the BOT via this link:

[https://lichess.org/@/Briwats](https://lichess.org/@/Briwats)

---

### Features:

- Basic NNUE
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

<!-- ### TODO:

- [ ] Make sure the nnue works (use some basic FENs)
- [ ] Write the article about NNUE
- [ ] Use the nnue :) (shockers)
- [ ] Output bucket nnue -->