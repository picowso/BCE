// #pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#include "header.hpp"
vector<move> Moves;
vector<move> EPMv;
pos KPoses[2];
int ind[255] = {};
int CNT = 0;
void debug(BBT Board, int i) {
	cout << minimax(Board, 0, 0, -1000, 1000).S << endl;
	// exit(0);
	// vector<BBT> vc = boardgen(Board, i);
	// cout << vc.size() << endl;
	// for(int i = 0 ; i < vc.size() ; i++) {
	// 	cout << i << endl;
	// 	displ(vc[i]);
	// }

	cout << CNT << endl;
	exit(0);
}

int main() {
	BBT Board;
	build_board(Board);
	// debug(Board, 0);
	ind['K'] = 1; ind['B'] = 2; ind['R'] = 3; ind['Q'] = 4;
	for(int i = 0 ; ; i++) {
		displ(Board);
		vector<BBT> vc = boardgen(Board, i);
		if(i&1) {
			cout << "Thinking...\n";
			debug(Board, i);
			int mv = minimax(Board, 0, i, -1000, 1000).F;
			Board = vc[mv];
			continue;
		}

	    movegen(Board, i);
	    int kj = Moves.size();
	    vector<move> fMoves = Moves;
	    for(move k: EPMv) fMoves.push_back(k);
	    // for(move k: fMoves) {
	    //     cout << k.F.F << " " << k.F.S << " " << k.S.F << " " << k.S.S << endl;
	    // }
	    
	    // no checks on a and b, please be civil and write (a-z)(1-8) :pray:
		cout << "Choose a move:\n";
	    string a, b; cin >> a >> b;
	    // Short castle
		if(a == "SC" and b == "SC") {
		    movegen(Board, i+1);
		    bool u = 1;
		    int rank = 7*(!(i&1));
		    for(move k: Moves) {
		        if(k.S.F == rank and 4 <= k.S.S and k.S.S <= 6) u = 0;
		    }

		    // empty
		    bool w = 1;
		    for(int j = 5 ; j <= 6 ; j++) {
		        if(Board[rank][j].type != VIDE) w = 0;
		    }

		    // cout << "H " << u << " " << w << endl;
		    // no moves
		    w = w and max(Board[rank][4].lstm, Board[rank][7].lstm) == -1;
		    if(w and u) {
		        // BBT nBoard = Board;
		        // copy(Board.begin(), Board.end(), nBoard);
		        Board[rank][5] = Board[rank][7];
		        Board[rank][6] = Board[rank][4];
		        Board[rank][5].lstm = Board[rank][6].lstm = i;
		        // Board[rank][5].numm++;
		        // Board[rank][6].numm++;
		        Board[rank][4] = Board[rank][7] = EMP;
		        // displ();
		    }

		    else {
		    	cout << "INVALID MOVE\n";
		    	i--;
		    	continue;
		    }
		}

	    // Long Castle
	    if(a == "LC" and b == "LC") {
		    int u = 1;
		    int rank = 7*(!(i&1));
		    for(move k: Moves) {
		        if(k.S.F == rank and 2 <= k.S.S and k.S.S <= 4) u = 0;
		    }

		    // empty
		    int w = 1;
		    for(int j = 1 ; j <= 3 ; j++) {
		        if(Board[rank][j].type != VIDE) w = 0;
		    }

		    // cout << "H " << u << " " << w << endl;
		    // no moves
		    w = w and max(Board[rank][0].lstm, Board[rank][4].lstm) == -1;
		    if(w and u) {
		        // BBT nBoard = Board;
		        // copy(Board, Board + 64, nBoard);
		        // copy(Board.begin(), Board.end(), nBoard);
		        Board[rank][2] = Board[rank][4];
		        Board[rank][3] = Board[rank][0];
		        Board[rank][2].lstm = Board[rank][3].lstm = i;
		        // Board[rank][2].numm++;
		        // Board[rank][3].numm++;
		        Board[rank][0] = Board[rank][4] = EMP;
		        // displ();
		    }

		    else {
		    	cout << "INVALID MOVE\n";
		    	i--;
		    	continue;
		    }
		}

	    move g = {{7 - (a[1] - '1'), a[0] - 'a'}, {7-(b[1] - '1'), b[0] - 'a'}};
	    cout << g.F.F << " " << g.F.S << " " << g.S.F << " " << g.S.S << endl;
	    bool fnd = 0;
	    for(int j = 0 ; j < fMoves.size() ; j++) {
	    	move pl = fMoves[j];
	    	if(g == pl) {
	    		fnd = 1;
		        BBT olbrd = Board;
		        // copy(Board, Board + 64, olbrd);
		        // copy(Board.begin(), Board.end(), olbrd);

		        int clr = 1;
		        if(!(i&1)) clr *= -1;
		        Board[pl.F.F][pl.F.S] = EMP;
		        Board[pl.S.F][pl.S.S] = olbrd[pl.F.F][pl.F.S];
		        Board[pl.S.F][pl.S.S].lstm = i;
		        // Board[pl.S.F][pl.S.S].numm++;

		        if(j >= kj) {
		            Board[pl.S.F-clr][pl.S.S] = EMP; // you ate in en passant :P
		        }

		        // king in check after the move?
		        if(incheck(Board, i)) {
		        	fnd = 0;
		        	Board = olbrd;
		        }

		        // pawn promo
		        int q = 7*(i&1);
	            if(Board[pl.S.F][pl.S.S].type%6 == 0 and pl.S.F == q) {
	                cout << "CHOOSE A PIECE TO PROMOTE PAWN (K, B, R, Q): " << endl;
	                char c; cin >> c;
	                int val = ind[c];
	                Board[pl.S.F][pl.S.S].type = (Type)(val + (i&1)*6);
	            }

		        // copy(olbrd, olbrd + 64, Board);
		        // copy(olbrd.begin(), olbrd.end(), Board);
	            break;
		    }
	    }

	    if(!fnd) {
	    	cout << "ILLEGAL MOVE\n";
	    	i--;
	    	continue;
	    }

	    // displ(Board);
	}
}
