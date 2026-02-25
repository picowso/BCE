#include "header.hpp"
extern BBT Board;
extern vector<move> Moves;
extern vector<move> EPMv;

// goal: 5
// notice that we keep generating the *whole* moves each time, cache them instead.
const int DEPTH_LIM = 5;

// a hand-crafted evaluation by me
//
// TODO:
// - involve NNs
// - tweak it more to increase elo :pray:
int evaluate(BBT CBoard, int mvnm) {
	int cw = 0, cb = 0;
	int lc[2] = {0};
	for(int i = 0 ; i < 8 ; i++) {
		for(int j = 0 ; j < 8 ; j++) {
			if(CBoard[i][j].type == VIDE) continue;
			int tp = CBoard[i][j].type%6;
			bool color = CBoard[i][j].type/6;
			if(CBoard[i][j].type < 6) cw += tp+1;
			else cb += tp+1;

			if(tp == 0) {
				if(color) lc[1] += 6 - i + 6*(i == 1) + 3*(i == 2);
				else lc[0] += i - 1 + 6*(i == 6) + 3*(i == 5);
			}

			// uhhh center is better for knights right?
			if(tp == 1) lc[color] += abs(i-4) + abs(j-4);
			if(tp == 2) {
				// bishop is weird, its most useful as a sniper
				// so ima claim sides are better?
				lc[color] += max(abs(j), abs(j-7)) + max(abs(i), abs(i-7));
			}

			if(tp == 3) {
				// rook is good in sides too
				lc[color] += max(abs(j), abs(j-7)) + max(abs(i), abs(i-7));
			}

			if(tp == 4) {
				// queen good in center
				lc[color] += 3*(abs(i-4) + abs(j-4));
			}

			if(tp == 5) {
				// just stay in your place bro :pray:
				// this feels bad for endgames, but should work with enough depth traversal
				lc[color] += i == color*7;
			}
		}
	}

	return (17*cw - 17*cb) + lc[1] - lc[0];
}

// minimax!!
// alpha beta!!
pair<int, int> minimax(BBT CBoard, int depth, int movn, int alpha, int beta) {
	if(depth == DEPTH_LIM) {
		// displ(CBoard);
		return {0, evaluate(CBoard, movn)};
	}

	vector<BBT> moves = boardgen(CBoard, movn);
	if(moves.empty()) {
		// displ(CBoard);
		if(incheck(CBoard, movn)) return {0, 100'000 * (depth&1 ? -1 : 1)}; // checkmate
		return {0, 0}; // stalemate
	}

	int nxtmv = 0;
	int sc = minimax(moves[0], depth+1, movn+1, alpha, beta).S;
	if(depth&1) {
		for(int i = 1 ; i < moves.size() ; i++) {
			int nsc = minimax(moves[i], depth+1, movn+1, alpha, beta).S;
			if(nsc < sc) {
				sc = nsc;
				nxtmv = i;
			}

			beta = min(beta, sc);
			if(beta <= alpha) break;
		}
	}
	
	else {
		for(int i = 1 ; i < moves.size() ; i++) {
			int nsc = minimax(moves[i], depth+1, movn+1, alpha, beta).S;
			if(nsc > sc) {
				sc = nsc;
				nxtmv = i;
			}

			alpha = max(alpha, sc);
			if(beta <= alpha) break;
		}
	}

	return {nxtmv, sc};
}