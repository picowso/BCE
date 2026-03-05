// engine.cpp: where the engine lives
#include "header.hpp"
extern BBT Board;
extern CMove Moves[218];
extern int mvs;
extern gp_hash_table<u64, u64> ztable;
gp_hash_table<u64, int> qsearch_v;
extern u64 zob_c;
// tables from https://www.chessprogramming.org/Simplified_Evaluation_Function
int PV[7][8][8] = { 0,  0,  0,  0,  0,  0,  0,  0,
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
    const int Pvals[6] = {250, 600, 650, 1000, 2000, 0};
    int score_w = 0, score_b = 0;
    for(int i = 0 ; i < 128 ; i++) {
		int r = i >> 4;
        int f = i & 7;
        if(i&0x88) continue;
        if(Board[i] == EMP) continue;
        if(color(i)) {
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

// int perft = 0;
int perft(int depth, bool turn) {
	// if(depth == 5) printb();
	// printb();
	if(depth == DEPTH_LIMIT) return 1;
	movegen(turn);
	int s = 0;
	vector<CMove> local(Moves, Moves + mvs);
	for(int i = 0 ; i < local.size() ; i++) {
		domove(local[i], 1);
		// if(local[i].flag) printb();
		int m = perft(depth + 1, turn ^ 1);
		if(depth == 0) cout << conv(local[i]) << " " << m << endl;
		s += m;
		undomove();
	}

	return s;
}

int perft_mm = 0;
extern CMove IND;
int quiescence(bool turn, int alpha, int beta) {
	perft_mm++;
	if(qsearch_v.find(zob_c) != qsearch_v.end()) return qsearch_v[zob_c];
	if(ztable[zob_c] >= 3) return 0;
	int bs = evaluate();
	if(turn) {
		if(bs >= beta) return beta;
		alpha = max(alpha, bs);
	}

	else {
		if(bs <= alpha) return alpha;
		beta = min(beta, bs);
	}

	movegen(turn);
	if(mvs == 0) {
		if(bincheck(turn)) return (turn ? -INF : INF);
		else return 0;
	}

	vector<CMove> local(Moves, Moves + mvs);
	sort(local.begin(), local.end(), [&](CMove a, CMove b) {
		return Pvals[a.capture] < Pvals[b.capture];
	});

	for(int i = 0 ; i < local.size() ; i++) {
		if(local[i].capture == EMP) continue;
		domove(local[i], 1);
		int ls = quiescence(turn^1, alpha, beta);
		undomove();
		if(turn) {
			alpha = max(alpha, ls);
			if(alpha >= beta) return alpha;
		}

		else {
			beta = min(beta, ls);
			if(beta <= alpha) return beta;
		}
	}

	return qsearch_v[zob_c] = bs;
}

int minimax(int depth, bool turn, int alpha, int beta) {
	if(depth == DEPTH_LIMIT) return quiescence(turn, alpha, beta);
	if(perft_mm > ITER_LIMIT) return quiescence(turn, alpha, beta);
	if(ztable[zob_c] >= 3) return 0;
	perft_mm++;
	int bs = (turn ? -INF : INF);
	movegen(turn);
	if(mvs == 0) {
		if(bincheck(turn)) return (turn ? -INF + depth : INF - depth);
		else return 0;
	}

	vector<CMove> local(Moves, Moves + mvs);
	// for(int i = 0 ; i < mvs ; i++) local[i] = Moves[i];
	// sort(local.begin(), local.end(), [&](CMove a, CMove b) {
	// 	return Pvals[a.capture] < Pvals[b.capture];
	// });

	for(int i = 0 ; i < local.size() ; i++) {
		domove(local[i], 1);
		int ls = minimax(depth + 1, turn ^ 1, alpha, beta);
		undomove();
		if(turn) {
			if(ls > bs) {
				if(depth == 0) IND = local[i];
				bs = ls;
			}

			alpha = max(alpha, bs);
			if(beta <= alpha) break;
		}

		else {
			if(ls < bs) {
				if(depth == 0) IND = local[i];
				bs = ls;
			}

			beta = min(beta, bs);
			if(beta <= alpha) break;
		}
	}

	return bs;
}