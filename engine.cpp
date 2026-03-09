// engine.cpp: where the engine lives
#include "header.hpp"
extern BBT Board;
extern CMove Moves[218];
extern int mvs;
extern gp_hash_table<u64, u64> ztable;
gp_hash_table<u64, int> qsearch_v;
// unordered_map<pair<int, int>, int> s_table;

extern u64 zob_c;
// tables stolen from https://github.com/thomasahle/sunfish/blob/master/sunfish.py#L24
int PV[6][8][8] = {0,   0,   0,   0,   0,   0,   0,   0,
            78,  83,  86,  73, 102,  82,  85,  90,
             7,  29,  21,  44,  40,  31,  44,   7,
           -17,  16,  -2,  15,  14,   0,  15, -13,
           -26,   3,  10,   9,   6,   1,   0, -23,
           -22,   9,   5, -11, -10,  -2,   3, -19,
           -31,   8,  -7, -37, -36, -14,   3, -31,
             0,   0,   0,   0,   0,   0,   0,   0,
     		-66, -53, -75, -75, -10, -55, -58, -70,
            -3,  -6, 100, -36,   4,  62,  -4, -14,
            10,  67,   1,  74,  73,  27,  62,  -2,
            24,  24,  45,  37,  33,  41,  25,  17,
            -1,   5,  31,  21,  22,  35,   2,   0,
           -18,  10,  13,  22,  18,  15,  11, -14,
           -23, -15,   2,   0,   2,   0, -23, -20,
           -74, -23, -26, -24, -19, -35, -22, -69,
     		-59, -78, -82, -76, -23,-107, -37, -50,
           -11,  20,  35, -42, -39,  31,   2, -22,
            -9,  39, -32,  41,  52, -10,  28, -14,
            25,  17,  20,  34,  26,  25,  15,  10,
            13,  10,  17,  23,  17,  16,   0,   7,
            14,  25,  24,  15,   8,  25,  20,  15,
            19,  20,  11,   6,   7,   6,  20,  16,
            -7,   2, -15, -12, -14, -15, -10, -10,
      		35,  29,  33,   4,  37,  33,  56,  50,
            55,  29,  56,  67,  55,  62,  34,  60,
            19,  35,  28,  33,  45,  27,  25,  15,
             0,   5,  16,  13,  18,  -4,  -9,  -6,
           -28, -35, -16, -21, -13, -29, -46, -30,
           -42, -28, -42, -25, -25, -35, -26, -46,
           -53, -38, -31, -26, -29, -43, -44, -53,
           -30, -24, -18,   5,  -2, -18, -31, -32,
       		6,   1,  -8,-104,  69,  24,  88,  26,
            14,  32,  60, -10,  20,  76,  57,  24,
            -2,  43,  32,  60,  72,  63,  43,   2,
             1, -16,  22,  17,  25,  20, -13,  -6,
           -14, -15,  -2,  -5,  -1, -10, -20, -22,
           -30,  -6, -13, -11, -16, -11, -16, -27,
           -36, -18,   0, -19, -15, -15, -21, -38,
           -39, -30, -31, -13, -31, -36, -34, -42,
       		4,  54,  47, -99, -99,  60,  83, -62,
           -32,  10,  55,  56,  56,  55,  10,   3,
           -62,  12, -57,  44, -67,  28,  37, -31,
           -55,  50,  11,  -4, -19,  13,   0, -49,
           -55, -43, -52, -28, -51, -47,  -8, -50,
           -47, -42, -43, -79, -64, -32, -29, -32,
            -4,   3, -14, -50, -57, -18,  13,   4,
            17,  30,  -3, -14,   6,  -1,  40,  18};

// enum NType {
// 	PV = 0,
// 	CUT = 1,
// 	ALL = 2
// };

struct TB {
	int depth;
	int sc;
	NType type;
};

extern int wkpos, bkpos;
const int Pvals[13] = {100, 280, 320, 479, 929, 0, 100, 280, 320, 479, 929, 0, 0};
int evaluate() {
    int score_w = 0, score_b = 0;
    int mat_w = 0, mat_b = 0;
    for(int i = 0 ; i < 128 ; i++) {
    	if(i&0x88) continue;
        if(Board[i] == EMP) continue;
		int r = i >> 4;
        int f = i & 7;
        if(color(i)) {
        	mat_w += Pvals[Board[i]];
        	score_w += PV[Board[i]][r][f];
        }

        else {
        	mat_b += Pvals[Board[i]];
        	score_b += PV[Board[i]-6][7-r][f];
        }
    }

    // endgame!
    // if(min(mat_w, mat_b) <= 1321) {
    // 	score_w -= PV[5][wkpos >> 4][wkpos & 7];
    // 	score_b -= PV[5][bkpos >> 4][bkpos & 7];
    // 	score_w += PV[6][wkpos >> 4][wkpos & 7];
    // 	score_b += PV[6][bkpos >> 4][bkpos & 7];
    // }

    return mat_w - mat_b + score_w - score_b;
}

// int perft = 0;
int perft(int depth, bool turn) {
	// if(depth == 5) printb();
	// printb();
	movegen(turn);
	if(depth == 1) return mvs;
	int s = 0;
	vector<CMove> local(Moves, Moves + mvs);
	for(int i = 0 ; i < local.size() ; i++) {
		domove(local[i]);
		// if(local[i].flag) printb();
		int m = perft(depth - 1, turn ^ 1);
		if(depth == 0) cout << conv(local[i]) << " " << m << endl;
		s += m;
		undomove();
	}

	return s;
}

int perft_mm = 0;
int tot = 0;
extern CMove IND;
int mtdf(bool turn, int f, int depth) {
	int l = -INF;
	int r = INF;
	while(l < r) {
		int beta = f + (f == l);
		f = minimax(depth, depth, beta-1, beta);
		if(f < beta) l = f;
		else r = f;
	};

	return f;
}

int quiescence(int depth, bool turn, int alpha, int beta) {
	perft_mm++;
	tot++;
	// if(qsearch_v.find(zob_c) != qsearch_v.end()) return qsearch_v[zob_c];
	// if(tot > ITER_LIMIT) {
	// 	qsearch_v.clear();
	// 	tot = 0;
	// }

	// if(ztable[zob_c] >= 3) return 0;
	auto it = ztable.find(zob_c);
	if (it != ztable.end() && it->second >= 3) return 0;
	int bs = evaluate();
	if(turn) {
		if(bs >= beta) return beta;
		alpha = max(alpha, bs);
	}

	else {
		if(bs <= alpha) return alpha;
		beta = min(beta, bs);
	}

	// bool check = incheck(turn);
	build_attack(!turn);
	movegen(turn);
	if(mvs == 0) {
		if(incheck(turn)) return (turn ? -INF - depth : INF + depth);
		else return 0;
	}

	vector<CMove> local(Moves, Moves + mvs);
	sort(local.rbegin(), local.rend(), [&](CMove a, CMove b) {
		int av = Pvals[a.capture];
		int bv = Pvals[b.capture];
		return av < bv;
	});

	for(int i = 0 ; i < local.size() ; i++) {
		if(local[i].capture == EMP and local[i].flag != 1) continue;
		domove(local[i]);
		int ls = quiescence(depth-1, turn^1, alpha, beta);
		undomove();
		if(turn) {
			bs = max(ls, bs);
			alpha = max(alpha, ls);
			if(alpha >= beta) return alpha;
		}

		else {
			bs = min(ls, bs);
			beta = min(beta, ls);
			if(beta <= alpha) return beta;
		}
	}

	return bs;
}

int minimax(int depth_n, int depth, bool turn, int alpha, int beta) {
	if(depth == 0) return quiescence(depth, turn, alpha, beta);
	// if(perft_mm > ITER_LIMIT) return quiescence(turn, alpha, beta);
	// if(ztable[zob_c] >= 3) return 0;
	auto it = ztable.find(zob_c);
	if (it != ztable.end() && it->second >= 3) return 0;
	// if(s_table.find({zob_c, depth}) != s_table.end()) return s_table[{zob_c, depth}];
	// perft_mm++;
	int bs = (turn ? -INF - 50 : INF + 50);
	// bool check = incheck(turn);
	build_attack(!turn);
	movegen(turn);
	for(int i = 0 ; i < mvs ; i++) if(depth == 0 and (Moves[i].flag==2 or Moves[i].flag==3)) cout << conv(Moves[i]) << endl;
	if(mvs == 0) {
		if(incheck(turn)) return (turn ? -INF - depth : INF + depth);
		else return 0;
	}

	vector<CMove> local(Moves, Moves + mvs);
	// for(int i = 0 ; i < mvs ; i++) local[i] = Moves[i];
	sort(local.rbegin(), local.rend(), [&](CMove a, CMove b) {
		int av = Pvals[a.capture];
		int bv = Pvals[b.capture];
		if(av != bv) return av < bv;

		int ai = Board[a.from], bi = Board[b.from];
		int aj = bincheck(a.from) * Pvals[ai], bj = bincheck(b.from) * Pvals[bi];
		if(aj != bj) return aj < bj;
		if(ai>5)ai-=6;
		if(bi>5)bi-=6;
		return (PV[ai][a.from >> 4][a.from & 7] - PV[ai][a.to >> 4][a.to & 7]) > (PV[bi][b.from >> 4][b.from & 7] - PV[bi][b.to >> 4][b.to & 7]);
	});

	for(int i = 0 ; i < local.size() ; i++) {
		domove(local[i]);
		int ls = minimax(depth_n, depth - 1, turn ^ 1, alpha, beta);
		undomove();
		if(turn) {
			if(ls > bs) {
				if(depth == depth_n) IND = local[i];
				bs = ls;
			}

			alpha = max(alpha, bs);
			if(beta <= alpha) break;
		}

		else {
			if(ls < bs) {
				if(depth == depth_n) IND = local[i];
				bs = ls;
			}

			beta = min(beta, bs);
			if(beta <= alpha) break;
		}
	}

	// return s_table[{zob_c, depth}] = bs;
	return bs;
}