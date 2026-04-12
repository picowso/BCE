### Plans:

So just starting out first with the crazy optimizations seems kinda unoptimal (mentally, for me) so what i'll do first is have a normal MLP feed-forward for each board (no accumulators no buckets no nothing)

An issue that i did in the first time i left the MLP to train is forgetting to encode which player should play in a board, since both players can have mate-in-1 if we change who can play!

Also since input size is exactly 768 which divides 64 (yay) i can use unsigned long long instead of very very long shit i was doing before, which would reduce training data size from 2gb to like 100mb hopefully :3