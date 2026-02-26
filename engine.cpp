#include "header.hpp"
// extern BBT Board;
extern vector<move> Moves;
extern vector<move> EPMv;
stack<vector<fmov>> fmoves;
const int DEPTH_LIM = 5;

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



// a hand-crafted evaluation by me
//
// TODO:
// - involve NNs
// - tweak it more to increase elo :pray:
int evaluate(BBT CBoard) {
	int cw = 0, cb = 0;
	int lc[2] = {0};
	int val[6] = {1, 3, 3, 5, 10, 0};
	for(int i = 0 ; i < 8 ; i++) {
		for(int j = 0 ; j < 8 ; j++) {
			if(CBoard[i][j].type == VIDE) continue;
			int tp = CBoard[i][j].type%6;
			bool color = CBoard[i][j].type/6;
			if(CBoard[i][j].type < 6) cw += val[tp];
			else cb += val[tp];

			int k = i;
			if(color) k = 7 - i;
			if(tp == 0) lc[color] += PV[k][j];
			if(tp == 1) lc[color] += MV[k][j];
			if(tp == 2) lc[color] += BV[k][j];
			if(tp == 3) lc[color] += RV[k][j];
			if(tp == 4) lc[color] += QV[k][j];
			if(tp == 5) lc[color] += KV[k][j];
		}
	}

	return 1000*(cw - cb) + lc[0] - lc[1];
}

// minimax!!
// alpha beta!!
pair<int, int> minimax(BBT &CBoard, int depth, int movn, int alpha, int beta) {
	if(depth == DEPTH_LIM) {
		// displ(CBoard);
		return {0, -evaluate(CBoard)};
	}

	vector<vector<fmov>> moves = bmovesgen(CBoard, movn);
	if(moves.empty()) {
		// displ(CBoard);
		if(incheck(CBoard, movn)) return {0, 100'000 * (depth&1 ? 1 : -1)}; // checkmate
		return {0, 0}; // stalemate
	}

	int nxtmv = 0;
	domove(CBoard, 1, moves[0]);
	int sc = minimax(CBoard, depth+1, movn+1, alpha, beta).S;
	undomove(CBoard);
	if(depth&1) {
		for(int i = 1 ; i < moves.size() ; i++) {
			domove(CBoard, 1, moves[i]);
			int nsc = minimax(CBoard, depth+1, movn+1, alpha, beta).S;
			undomove(CBoard);
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
			domove(CBoard, 1, moves[i]);
			int nsc = minimax(CBoard, depth+1, movn+1, alpha, beta).S;
			undomove(CBoard);
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

// suicidal knight eval :praycat:
// int evaluatev1(BBT CBoard, int mvnm) {
// 	int cw = 0, cb = 0;
// 	int lc[2] = {0};
// 	for(int i = 0 ; i < 8 ; i++) {
// 		for(int j = 0 ; j < 8 ; j++) {
// 			if(CBoard[i][j].type == VIDE) continue;
// 			int tp = CBoard[i][j].type%6;
// 			bool color = CBoard[i][j].type/6;
// 			if(CBoard[i][j].type < 6) cw += tp+1;
// 			else cb += tp+1;

// 			if(tp == 0) {
// 				if(color) lc[1] += 3*(6 - i) + 10*(i == 1) + 6*(i == 2);
// 				else lc[0] += 3*(i - 1) + 10*(i == 6) + 6*(i == 5);
// 			}

// 			// uhhh center is better for knights right?
// 			// if(tp == 1) lc[color] += (abs(i-4) + abs(j-4))/2;
// 			if(tp == 2) {
// 				// bishop is weird, its most useful as a sniper
// 				// so ima claim sides are better?
// 				lc[color] += max(abs(j), abs(j-7))/2 + max(abs(i), abs(i-7))/2;
// 			}

// 			if(tp == 3) {
// 				// rook is good in sides too
// 				lc[color] += max(abs(j), abs(j-7))/2 + max(abs(i), abs(i-7))/2;
// 			}

// 			if(tp == 4) {
// 				// queen good in center
// 				lc[color] += 3*(abs(i-4) + abs(j-4));
// 			}

// 			if(tp == 5) {
// 				// just stay in your place bro :pray:
// 				// this feels bad for endgames, but should work with enough depth traversal
// 				lc[color] += i == color*7;
// 			}
// 		}
// 	}

// 	return (17*cw - 17*cb) + lc[1] - lc[0];
// }