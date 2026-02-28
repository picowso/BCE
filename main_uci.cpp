#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#include "header.hpp"
vector<move> Moves;
vector<move> EPMv;
pos KPoses[2];
int CNT = 0;
int ind[255] = {};

// https://gist.github.com/DOBRO/2592c6dad754ba67e6dcaec8c90165bf
int main() {
	BBT Board;
	build_board(Board);
	vector<string> gg = ucimovesgen(Board, 0);
	// for(string g: gg) cout << g << endl;
	int movn = 0;
	cout << "id name BriwatsCE\n";
	cout << "id author Anass Zakar\n";
	cout << "uciok\n";
	cout << flush;
	// bool is_white = 0;
	for(;;) {
		string inp;
		// cin >> inp;
		getline(cin, inp);
		// cout << inp.substr(0, 23) << endl;
		if(inp == "quit") {
			return 0;
		}

		else if(inp == "uci") {
			cout << "id name BriwatsCE\n";
			cout << "id author Anass Zakar\n";
			cout << "uciok\n";
			cout << flush;
		}

		else if(inp == "isready") {
			cout << "readyok" << endl;;
		}

		else if(inp == "ucinewgame") {
			build_board(Board);
		}

		// bug is there is some global var that isnt cleared
		else if(inp.substr(0, 23) == "position startpos moves") {
			string u;
			for(int i = inp.size()-1 ; i >= 0 ; i--) {
				if(inp[i] == ' ') break;
				u.push_back(inp[i]);
			}

			reverse(u.begin(), u.end());
			// in case its white :3
			vector<string> cur = ucimovesgen(Board, movn);
			vector<vector<fmov>> act = bmovesgen(Board, movn);
			bool fnd = 0;
			// cout << u << endl;
			for(int j = 0 ; j < cur.size() ; j++) {
				// cout << cur[j] << " ";
				if(cur[j] == u) {
					fnd = 1;
					domove(Board, 0, act[j], movn);
					// displ(Board);
				}
			}

			if(!fnd) {
				cout << u << "ERROR :3" << endl;
				exit(0);
			}

			// displ(Board);
			movn++;
		}

		else if(inp.substr(0, 2) == "go") {
			vector<string> cur = ucimovesgen(Board, movn);
			vector<vector<fmov>> act = bmovesgen(Board, movn);
			int ind = minimax(Board, 0, movn, -INF, INF).F;
			cout << "bestmove " << cur[ind] << endl;
			domove(Board, 0, act[ind]);
			movn++;
		}
	}
}