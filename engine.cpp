// engine.cpp: where the engine lives
#include "header.hpp"
extern BBT Board;
extern CMove Moves[218];
extern int mvs;
extern u8 color[128];
extern gp_hash_table<u64, u64> ztable;
extern u64 zob_c;
// tables from https://www.chessprogramming.org/Simplified_Evaluation_Function
int PV[8][8][8] = { 0,  0,  0,  0,  0,  0,  0,  0,
				50, 50, 50, 50, 50, 50, 50, 50,
				10, 10, 20, 30, 30, 20, 10, 10,
				 5,  5, 10, 25, 25, 10,  5,  5,
				 0,  0,  0, 20, 20,  0,  0,  0,
				 5, -5,-10,  0,  0,-10, -5,  5,
				 5, 10, 10,-20,-20, 10, 10,  5,
				 0,  0,  0,  0,  0,  0,  0,  0,
				-50,-40,-30,-30,-30,-30,-40,-50,
				-40,-20,  0,  0,  0,  0,-20,-40,
				-30,  0, 10, 15, 15, 10,  0,-30,
				-30,  5, 15, 20, 20, 15,  5,-30,
				-30,  0, 15, 20, 20, 15,  0,-30,
				-30,  5, 10, 15, 15, 10,  5,-30,
				-40,-20,  0,  5,  5,  0,-20,-40,
				-50,-40,-30,-30,-30,-30,-40,-50,
				-20,-10,-10,-10,-10,-10,-10,-20,
				-10,  0,  0,  0,  0,  0,  0,-10,
				-10,  0,  5, 10, 10,  5,  0,-10,
				-10,  5,  5, 10, 10,  5,  5,-10,
				-10,  0, 10, 10, 10, 10,  0,-10,
				-10, 10, 10, 10, 10, 10, 10,-10,
				-10,  5,  0,  0,  0,  0,  5,-10,
				-20,-10,-10,-10,-10,-10,-10,-20,
				0,  0,  0,  0,  0,  0,  0,  0,
				  5, 10, 10, 10, 10, 10, 10,  5,
				 -5,  0,  0,  0,  0,  0,  0, -5,
				 -5,  0,  0,  0,  0,  0,  0, -5,
				 -5,  0,  0,  0,  0,  0,  0, -5,
				 -5,  0,  0,  0,  0,  0,  0, -5,
				 -5,  0,  0,  0,  0,  0,  0, -5,
				  0,  0,  0,  5,  5,  0,  0,  0,
				-20,-10,-10, -5, -5,-10,-10,-20,
				-10,  0,  0,  0,  0,  0,  0,-10,
				-10,  0,  5,  5,  5,  5,  0,-10,
				 -5,  0,  5,  5,  5,  5,  0, -5,
				  0,  0,  5,  5,  5,  5,  0, -5,
				-10,  5,  5,  5,  5,  5,  0,-10,
				-10,  0,  5,  0,  0,  0,  0,-10,
				-20,-10,-10, -5, -5,-10,-10,-20,
				-30,-40,-40,-50,-50,-40,-40,-30,
				-30,-40,-40,-50,-50,-40,-40,-30,
				-30,-40,-40,-50,-50,-40,-40,-30,
				-30,-40,-40,-50,-50,-40,-40,-30,
				-20,-30,-30,-40,-40,-30,-30,-20,
				-10,-20,-20,-20,-20,-20,-20,-10,
				 20, 20,  0,  0,  0,  0, 20, 20,
				 20, 30, 10,  0,  0, 10, 30, 20,
				-50,-40,-30,-20,-20,-30,-40,-50,
				-30,-20,-10,  0,  0,-10,-20,-30,
				-30,-10, 20, 30, 30, 20,-10,-30,
				-30,-10, 30, 40, 40, 30,-10,-30,
				-30,-10, 30, 40, 40, 30,-10,-30,
				-30,-10, 20, 30, 30, 20,-10,-30,
				-30,-30,  0,  0,  0,  0,-30,-30,
				-50,-30,-30,-30,-30,-30,-30,-50};

int evaluate() {
    const int Pvals[6] = {100, 200, 450, 500, 1000, 0};
    int score_w = 0, score_b = 0;
    for(int i = 0 ; i < 128 ; i++) {
		int r = i >> 4;
        int f = i & 7;
        if(i&0x88) continue;
        if(Board[i] == EMP) continue;
        if(color[i]) {
        	score_w += Pvals[Board[i]];
        	score_w += PV[Board[i]][r][f];
        }

        else {
        	score_b += Pvals[Board[i]];
        	score_b += PV[Board[i]][7-r][f];
        }
    }

    return score_w - score_b;
}

int perft = 0;
array<short, 2> minimax(int depth, bool turn, int alpha, int beta) {
	// printb();
	if(depth == DEPTH_LIMIT) {
		return {-1, evaluate()};
	}
	perft++;

	if(ztable[zob_c] >= 3) {
		return {-1, 0};
	}

	movegen(turn);
	int bscore = INF * (turn ? -1 : 1 );
	u8 ind = 255;
	// cout << depth << " " << turn << endl;
	int sz = mvs;
	CMove local[sz];
	memcpy(local, Moves, mvs * sizeof(CMove));
	for(int i = 0 ; i < sz ; i++) {
		domove(local[i], 1);
		array<short, 2> cscore = minimax(depth + 1, turn ^ 1, alpha, beta);
		undomove();

		if(turn) {
			if(cscore[1] > bscore) {
				bscore = cscore[1];
				ind = i;
			}

			if(cscore[1] >alpha) alpha = cscore[1];
			// if(beta <= alpha) break;
		}

		else {
			if(cscore[1] < bscore) {
				bscore = cscore[1];
				ind = i;
			}

			if(beta > cscore[1]) beta = cscore[1];
			// if(beta <= alpha) break;
		}
	}

	if(ind == 255) {
		if(incheck(turn)) return {0, (turn ? -INF : INF)};
		return {0, 0};
	}

	return {ind, bscore};
}