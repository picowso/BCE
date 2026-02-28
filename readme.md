# Briwats Chess Engine

a pseudo-functional and kinda unefficient chess engine

---

### what should be done:

- [X] Remove Board from being global and pass by reference instead

- [X] Optimize MG (move/unmove instead of storing boards!)

- [X] Implement UCI move notation

- [X] Implement UCI and hook it up with Cute-Chess or Lichess

- [X] Make it bug-less! (play a lot against it in Cute-Chess, should be sure after playing 3 full games without illegal moves from it)

- [ ] Implement Zobrist Hashing

- [ ] Use Mailbox or 0x88 instead of current 8x8

- [ ] Use a NNUE/NN/MLP/something instead of the hand-made evaluation function, a good dataset for this might be +3000 lichess rapid games.

##### small todos:

<!-- - [ ] Implement Threefold-Repetition-Draw check -->
<!-- The reason why we dont need this is it's handeled by UCI, we can instead just find the shortest path to the best eval! -->

- [X] Fix pawn promotion in UCI

- [X] Make it also play white in UCI

- [X] Fix en passant

- [X] Fix new bug (briwats always plays worst moves as white?)

- [ ] Read more of this https://www.chessprogramming.org/Move_Ordering

###### even smaller:

- [ ] reduce 1.5s in depth 5 to ~0.5s by optimizing movegen() choices