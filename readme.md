# Briwats Chess Engine

BriwatsCE is a UCI NNUE chess engine, it uses a 0x88 board representation and can either use MTD(f) or normal MiniMax with alpha beta (and TT!) for the search, i implemented chess from scratch which was a pain to code!

The NNUE is trained on the engine's self-play, it's still not stable and can produce catastrophic or random moves in some uncommon positions since it's only trained on ~30 Million games, this number (30M) should increase in future releases!

## How to play:

Make sure you have a [Lichess](https://lichess.org) account, and challenge the BOT via this link:

[https://lichess.org/@/Briwats](https://lichess.org/@/Briwats)

---

## How to replicate:

Make sure to have `git`, `make` and `g++` installed.

`git clone https://github.com/picowso/BCE`
`cd BCE`
`make`

now you'll have a binary named "briwats" which is UCI-compatible, you can either use `BCE-lichess` and import your own configurations to have your own bot on [Lichess](https://lichess.org), [see here on how to do that](https://github.com/lichess-bot-devs/lichess-bot) or use it locally using GUIs like [Cute Chess](https://cutechess.com/) which is what i personally use, just plug the binary in the "Tools" section and run it.

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

## sub-projects

[BCE-train](https://github.com/picowso/BCE-train): To train the NNUE, not that the NNUE is still not good.

[BCE-lichess](https://github.com/picowso/BCE-lichess): To have the bot on Lichess, this is a fork of [lichess-bot](https://github.com/lichess-bot-devs/lichess-bot).

<!-- ### TODO:

- [X] Make sure the nnue works (use some basic FENs)
- [ ] Write the article about NNUE
- [X] Use the nnue :) (shockers)
- [ ] Output bucket nnue -->