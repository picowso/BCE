#include "header.hpp"
extern vector<move> Moves;
extern vector<move> EPMv;
extern int CNT;
stack<vector<fmov>> fmoves;
const int DEPTH_LIM = 4;

// tables from https://www.chessprogramming.org/Simplified_Evaluation_Function
int PV[8][8] = 	{ 0,  0,  0,  0,  0,  0,  0,  0,
				50, 50, 50, 50, 50, 50, 50, 50,
				10, 10, 20, 30, 30, 20, 10, 10,
				 5,  5, 10, 25, 25, 10,  5,  5,
				 0,  0,  0, 20, 20,  0,  0,  0,
				 5, -5,-10,  0,  0,-10, -5,  5,
				 5, 10, 10,-20,-20, 10, 10,  5,
				 0,  0,  0,  0,  0,  0,  0,  0};

int MV[8][8] = 	{ -50,-40,-30,-30,-30,-30,-40,-50,
				-40,-20,  0,  0,  0,  0,-20,-40,
				-30,  0, 10, 15, 15, 10,  0,-30,
				-30,  5, 15, 20, 20, 15,  5,-30,
				-30,  0, 15, 20, 20, 15,  0,-30,
				-30,  5, 10, 15, 15, 10,  5,-30,
				-40,-20,  0,  5,  5,  0,-20,-40,
				-50,-40,-30,-30,-30,-30,-40,-50};

int BV[8][8] = 	{-20,-10,-10,-10,-10,-10,-10,-20,
				-10,  0,  0,  0,  0,  0,  0,-10,
				-10,  0,  5, 10, 10,  5,  0,-10,
				-10,  5,  5, 10, 10,  5,  5,-10,
				-10,  0, 10, 10, 10, 10,  0,-10,
				-10, 10, 10, 10, 10, 10, 10,-10,
				-10,  5,  0,  0,  0,  0,  5,-10,
				-20,-10,-10,-10,-10,-10,-10,-20};

int RV[8][8] = 	{ 0,  0,  0,  0,  0,  0,  0,  0,
				  5, 10, 10, 10, 10, 10, 10,  5,
				 -5,  0,  0,  0,  0,  0,  0, -5,
				 -5,  0,  0,  0,  0,  0,  0, -5,
				 -5,  0,  0,  0,  0,  0,  0, -5,
				 -5,  0,  0,  0,  0,  0,  0, -5,
				 -5,  0,  0,  0,  0,  0,  0, -5,
				  0,  0,  0,  5,  5,  0,  0,  0};

int QV[8][8] = 	{-20,-10,-10, -5, -5,-10,-10,-20,
				-10,  0,  0,  0,  0,  0,  0,-10,
				-10,  0,  5,  5,  5,  5,  0,-10,
				 -5,  0,  5,  5,  5,  5,  0, -5,
				  0,  0,  5,  5,  5,  5,  0, -5,
				-10,  5,  5,  5,  5,  5,  0,-10,
				-10,  0,  5,  0,  0,  0,  0,-10,
				-20,-10,-10, -5, -5,-10,-10,-20};

int KV[8][8] = 	{-30,-40,-40,-50,-50,-40,-40,-30,
				-30,-40,-40,-50,-50,-40,-40,-30,
				-30,-40,-40,-50,-50,-40,-40,-30,
				-30,-40,-40,-50,-50,-40,-40,-30,
				-20,-30,-30,-40,-40,-30,-30,-20,
				-10,-20,-20,-20,-20,-20,-20,-10,
				 20, 20,  0,  0,  0,  0, 20, 20,
				 20, 30, 10,  0,  0, 10, 30, 20};

int EKV[8][8] = {-50,-40,-30,-20,-20,-30,-40,-50,
				-30,-20,-10,  0,  0,-10,-20,-30,
				-30,-10, 20, 30, 30, 20,-10,-30,
				-30,-10, 30, 40, 40, 30,-10,-30,
				-30,-10, 30, 40, 40, 30,-10,-30,
				-30,-10, 20, 30, 30, 20,-10,-30,
				-30,-30,  0,  0,  0,  0,-30,-30,
				-50,-30,-30,-30,-30,-30,-30,-50};

// TODO:
// - involve NNs
// - tweak it more to increase elo :pray:
int Pvals[13] = {10, 32, 33, 50, 100, 0, 10, 32, 33, 50, 100, 0, 0};
int evaluate(BBT CBoard) {
	int cw = 0, cb = 0;
	int lc[2] = {0};
	int val[6] = {1, 3, 3, 5, 10, 0};
	for(int i = 0 ; i < 8 ; i++) {
		for(int j = 0 ; j < 8 ; j++) {
			if(CBoard[i][j].type == VIDE) continue;
			int tp = CBoard[i][j].type;
			if(tp > 5) tp -= 6;
			bool color = CBoard[i][j].type/6;
			if(CBoard[i][j].type < 6) cw += Pvals[tp];
			else cb += Pvals[tp];

			int k = i;
			if(color) k = 7 - i;
			if(tp == 0) lc[color] += PV[k][j];
			if(tp == 1) lc[color] += MV[k][j];
			if(tp == 2) lc[color] += BV[k][j];
			if(tp == 3) lc[color] += RV[k][j];
			if(tp == 4) lc[color] += QV[k][j];
			if(tp == 5) {
				if(min(cw, cb) > 100) lc[color] += KV[k][j];
				else lc[color] += EKV[k][j];
			}
		}
	}

	return 100*(cw - cb) + lc[0] - lc[1];
}

// minimax!! alpha beta!!
array<int, 3> minimax(BBT &CBoard, int depth, int movn, int alpha, int beta) {
	CNT++;
	bool is_white = (movn-depth)&1;
	if(depth == DEPTH_LIM) {
		return {0, evaluate(CBoard)};
	}

	vector<vector<fmov>> moves = bmovesgen(CBoard, movn);
	if(moves.empty()) {
		if(incheck(CBoard, movn)) return {0, -INF, depth}; // checkmate
		return {0, 0}; // stalemate
	}

	// move ordering
	vector<int> movo(moves.size());
	for(int i = 0 ; i < moves.size() ; i++) movo[i] = i;
	sort(movo.begin(), movo.end(), [&](int i, int j) {
		int ai = get<0>(moves[i][0]), aj = get<1>(moves[i][0]);
		int bi = get<0>(moves[j][0]), bj = get<1>(moves[j][0]);
		uchar a = CBoard[ai][aj].type, b = CBoard[bi][bj].type;
		if(a==12)a=0;
		if(b==12)b=0;
		return a > b;
	});

	int nxtmv = movo[0];
	domove(CBoard, 1, moves[movo[0]], movn);
	array<int,3> gg = minimax(CBoard, depth+1, movn+1, alpha, beta);
	undomove(CBoard);
	int sc = gg[1];
	int dp = gg[2];
	if(movn&1) {
		for(int i = 1 ; i < moves.size() ; i++) {
			domove(CBoard, 1, moves[movo[i]], movn);
			array<int,3> uw = minimax(CBoard, depth+1, movn+1, alpha, beta);
			undomove(CBoard);
			if(uw[1] < sc) {
				dp = uw[2];
				sc = uw[1];
				nxtmv = movo[i];
			}

			if(uw[1] == sc and dp > uw[2]) {
				dp = uw[2];
				sc = uw[1];
				nxtmv = movo[i];
			}

			beta = min(beta, sc);
			if(beta <= alpha) break;
		}
	}
	
	else {
		for(int i = 1 ; i < moves.size() ; i++) {
			domove(CBoard, 1, moves[movo[i]], movn);
			array<int,3> uw = minimax(CBoard, depth+1, movn+1, alpha, beta);
			undomove(CBoard);
			if(uw[1] > sc) {
				dp = uw[2];
				sc = uw[1];
				nxtmv = movo[i];
			}

			if(uw[1] == sc and dp > uw[2]) {
				dp = uw[2];
				sc = uw[1];
				nxtmv = movo[i];
			}

			alpha = max(alpha, sc);
			if(beta <= alpha) break;
		}
	}

	return {nxtmv, sc, depth};
}