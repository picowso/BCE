#pragma GCC optimize("Ofast")
#pragma GCC optimize ("unroll-loops")
#include "header.hpp"
vector<move> Moves;
vector<move> EPMv;
pos KPoses[2];
int ind[255] = {};
int CNT = 0;

int main() {
	BBT Board;
	build_board(Board);
	ind['K'] = 1; ind['B'] = 2; ind['R'] = 3; ind['Q'] = 4;
	for(int i = 0 ; ; i++) {
		displ(Board);
		vector<BBT> vc = boardgen(Board, i);
		if(i&1) {
			cout << "Thinking...\n";
			int mv = minimax(Board, 0, i, -1000, 1000).F;
			Board = vc[mv];
			continue;
		}

		string u; cin >> u;
		vector<string> cur = ucimovesgen(Board, i);
		vector<vector<fmov>> act = bmovesgen(Board, i);
		bool fnd = 0;
		for(int j = 0 ; j < cur.size() ; j++) {
			if(cur[j] == u) {
				fnd = 1;
				domove(Board, 0, act[j], i);
			}
		}

		if(!fnd) {
			cout << u << "ERROR :3" << endl;
			exit(0);
		}
	}
}
