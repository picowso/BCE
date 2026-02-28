#pragma GCC optimize("Ofast")
#pragma GCC optimize("unroll-loops")
#include "header.hpp"
vector<move> Moves;
vector<move> EPMv;
pos KPoses[2];
int CNT = 0;
int ind[255] = {};

// reference: https://gist.github.com/DOBRO/2592c6dad754ba67e6dcaec8c90165bf
int main() {
	int movn = 0;
	BBT Board;
	build_board(Board);
	vector<string> gg = ucimovesgen(Board, 0);
	cout << "id name BriwatsCE\n";
	cout << "id author Anass Zakar\n";
	cout << "uciok\n";
	cout << flush;
	for(;;) {
		string inp;
		getline(cin, inp);
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

		else if(inp.substr(0, 23) == "position startpos moves") {
			string u;
			for(int i = inp.size()-1 ; i >= 0 ; i--) {
				if(inp[i] == ' ') break;
				u.push_back(inp[i]);
			}

			reverse(u.begin(), u.end());
			vector<string> cur = ucimovesgen(Board, movn);
			vector<vector<fmov>> act = bmovesgen(Board, movn);
			bool fnd = 0;
			for(int j = 0 ; j < cur.size() ; j++) {
				if(cur[j] == u) {
					fnd = 1;
					domove(Board, 0, act[j], movn);
				}
			}

			if(!fnd) {
				cout << u << "ERROR :3" << endl;
				exit(0);
			}

			movn++;
		}

		else if(inp.substr(0, 2) == "go") {
			vector<string> cur = ucimovesgen(Board, movn);
			vector<vector<fmov>> act = bmovesgen(Board, movn);
			int ind = minimax(Board, 0, movn, -INF, INF)[0];
			cout << "bestmove " << cur[ind] << endl;
			domove(Board, 0, act[ind]);
			movn++;
		}
	}
}