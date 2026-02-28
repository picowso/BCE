// #pragma GCC target("avx2")
#pragma GCC optimize("Ofast")
#pragma GCC optimize ("unroll-loops")
// #pragma GCC target("mmx")
#include "header.hpp"
vector<move> Moves;
vector<move> EPMv;
pos KPoses[2];
int ind[255] = {};
int CNT = 0;
void debug(BBT Board, int i) {
	cout << minimax(Board, 0, 0, -1000, 1000)[1] << endl;
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
	debug(Board, 0);
	ind['K'] = 1; ind['B'] = 2; ind['R'] = 3; ind['Q'] = 4;
	for(int i = 0 ; ; i++) {
		displ(Board);
		// vector<BBT> vc = boardgen(Board, i);
		// if(i&1) {
		// 	cout << "Thinking...\n";
		// 	debug(Board, i);
		// 	int mv = minimax(Board, 0, i, -1000, 1000).F;
		// 	Board = vc[mv];
		// 	continue;
		// }
		string u; cin >> u;
		vector<string> cur = ucimovesgen(Board, i);
		vector<vector<fmov>> act = bmovesgen(Board, i);
		bool fnd = 0;
		// cout << u << endl;
		for(int j = 0 ; j < cur.size() ; j++) {
			// cout << cur[j] << " ";
			if(cur[j] == u) {
				fnd = 1;
				domove(Board, 0, act[j], i);
				// displ(Board);
			}
		}

		if(!fnd) {
			cout << u << "ERROR :3" << endl;
			exit(0);
		}
	}
}
