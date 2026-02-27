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
			cout << endl;
		}

		else if(inp == "isready") {
			cout << "readyok" << endl;;
		}

		else if(inp == "ucinewgame") {
			build_board(Board);
		}

		else if(inp.substr(0, 23) == "position startpos moves") {
			build_board(Board);
			// cout << "bestmove b7b6" << endl;
			string u;
			vector<string> moves;
			inp.push_back(' ');
			for(int i = 23 ; i < inp.size() ; i++) {
				if(inp[i] == ' ') {
					if(u.size()) {
						moves.push_back(u);
						u = "";
					}
				}

				if(inp[i] != ' ') u.push_back(inp[i]);
			}

			int movn = moves.size();
			// if you think this is unefficient look at the rest of the code
			for(int i = 0 ; i < moves.size() ; i++) {
				vector<string> cur = ucimovesgen(Board, i);
				vector<vector<fmov>> act = bmovesgen(Board, i);
				bool fnd = 0;
				for(int j = 0 ; j < cur.size() ; j++) {
					if(cur[j] == moves[i]) {
						domove(Board, 0, act[j], i);
					}
				}
			}

			displ(Board);
			int ind = minimax(Board, 0, movn, -INF, INF).F;
			vector<string> cur = ucimovesgen(Board, movn);
			vector<vector<fmov>> act = bmovesgen(Board, movn);
			for(string u: cur) cout << u << endl;
			cout << act.size() << " " << cur.size() << " " << ind << endl;
			// debug
			// cout << act.size() << " " << ind << endl;
			cout << "bestmove " << cur[ind] << endl;
			// domove(Board, 1, act[ind]);
			// displ(Board);
			// cout << flush;
			// undomove(Board);
		}
	}
}