// main_uci.cpp: UCI interface for the engine
#include "header.hpp"
extern BBT Board;
extern CMove Moves[218];
extern int mvs;
extern gp_hash_table<u64, u64> ztable;
extern u64 zob_c;
// reference: https://gist.github.com/DOBRO/2592c6dad754ba67e6dcaec8c90165bf
bool mv = 0;
char str2[6] = {'p', 'n', 'b', 'r', 'q', 'k'};
string conv(CMove cmove) {
	string r = {'a' + (cmove.from & 7),
			'1' + (7 - (cmove.from >> 4)),
			'a' + (cmove.to & 7),
			'1' + (7 - (cmove.to >> 4))
	};

	if(cmove.promo != EMP) r.push_back(str2[cmove.promo]);
	return r;
}

extern u8 color[128]; // 0: black, 1: white, 2: nothing
extern u64 zobrist[128][7][3];
extern u64 zob_c;
extern gp_hash_table<u64, u64> ztable;
extern int wkpos, bkpos;
extern int perft;
CMove Lm = {0, 0, EMP, EMP, 0}; // last move!
int main() {
	build_board();
	// auto t0 = chrono::high_resolution_clock::now();
	// cout << minimax(0, 1)[1] << endl;
	// cout << perft << endl;
	// cout << perft(0, 3, 0) << endl;
	// for(int i = 1 ; i < 5 ; i++) cout << perft(0, i, 1) << endl;
	// auto t1 = chrono::high_resolution_clock::now();
	// cout << chrono::duration<double>(t1 - t0).count() << endl;
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
					Lm = Moves[i];
					domove(Moves[i], 0);
					break;
				}
			}

			// cout << "OMG " << ztable[zob_c] << endl;
		}

		else if(inp.substr(0, 2) == "go") {
			// printb();
			mv ^= 1;
			int ind = minimax(0, mv)[0];
			movegen(mv);
			// cout << mvs << endl;
			// for(int i = 0 ; i < mvs ; i++) {
			// 	CMove Move = Moves[i];
			// 	cout << conv(Move) << endl;
			// }

			Lm = Moves[ind];
			cout << "bestmove " << conv(Moves[ind]) << endl;
			domove(Moves[ind], 0);
			// cout << perft << " " << mvs << " " << ind << " OMG " << ztable[zob_c] << endl;
		}
	}
}