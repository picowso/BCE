### Plans:

So just starting out first with the crazy optimizations seems kinda unoptimal (mentally, for me) so what i'll do first is have a normal MLP feed-forward for each board (no accumulators no buckets no nothing)

An issue that i did in the first time i left the MLP to train is forgetting to encode which player should play in a board, since both players can have mate-in-1 if we change who can play, I'll rotate board each time and `eval *= -1` and that should be fixed.

Ill train on self-play by the engine itself since leela zero data seems to produce bad engines