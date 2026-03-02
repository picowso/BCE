// main_uci.cpp: UCI interface for the engine
#include "header.hpp"
extern BBT Board;
extern CMove Moves[218];
extern int mvs;
extern gp_hash_table<u64, u64> ztable;
extern u64 zob_c;
// reference: https://gist.github.com/DOBRO/2592c6dad754ba67e6dcaec8c90165bf
bool mv = 0;
string conv(CMove cmove) {
	return {'a' + (cmove.from & 7),
			'1' + (7 - (cmove.from >> 4)),
			'a' + (cmove.to & 7),
			'1' + (7 - (cmove.to >> 4))
	};
}

extern int perft;
int main() {
	build_board();
	// cout << minimax(0, 1)[0] << endl;
	// cout << perft << endl;
	// exit(0);
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
			build_board();
		}

		else if(inp.substr(0, 23) == "position startpos moves") {
			printb();
			mv ^= 1;
			string u;
			for(int i = inp.size()-1 ; i >= 0 ; i--) {
				if(inp[i] == ' ') break;
				u.push_back(inp[i]);
			}

			reverse(u.begin(), u.end());
			movegen(mv);
			for(int i = 0 ; i < mvs ; i++) {
				string c = conv(Moves[i]);
				if(c == u) {
					domove(Moves[i], 0);
					break;
				}
			}

			cout << "OMG " << ztable[zob_c] << endl;
		}

		else if(inp.substr(0, 2) == "go") {
			printb();
			mv ^= 1;
			int ind = minimax(0, mv)[0];
			// cout << perft << endl;
			movegen(mv);
			for(int i = 0 ; i < mvs ; i++) {
				// cout << i <<" " <<conv(Moves[i]) << endl;
			}

			cout << "bestmove " << conv(Moves[ind]) << endl;
			domove(Moves[ind], 0);
			cout << "OMG " << ztable[zob_c] << endl;
		}
	}
}