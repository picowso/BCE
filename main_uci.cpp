// TODO: support UCI
#include "header.hpp"
BBT Board;
vector<move> Moves;
vector<move> EPMv;
int ind[255] = {};

// BCE playing as black
int main() {
	cout << "id name BriwatsCE\n";
	cout << "id author Anass Zakar\n";
	cout << "uciok\n";
	for(;;) {
		string inp; cin >> inp;
		if(inp == "quit") {
			return 0;
		}

		else if(inp == "uci") {
			cout << "id name BriwatsCE\n";
			cout << "id author Anass Zakar\n";
			cout << "uciok\n";
		}

		else if(inp == "isready") {
			cout << "readyok\n";
		}

		else if(inp == "ucinewgame") {
			build_board();
		}

		else if(inp == "go depth 1") {
			cout << "bestmove d2d4\n";
		}

		else if(inp == "position startpos moves d2d4") {
			cout << "bestmove b7 b6\n";
		}
	}
}