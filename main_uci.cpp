// main_uci.cpp: UCI interface for the engine
#include "header.hpp"
extern BBT Board;
extern CMove Moves[218];
extern int mvs;
extern gp_hash_table<u64, u64> ztable;
extern u64 zob_c;
// reference: https://gist.github.com/DOBRO/2592c6dad754ba67e6dcaec8c90165bf
bool mv = 1;
char str2[6] = {'p', 'n', 'b', 'r', 'q', 'k'};
string conv(CMove cmove) {
	string r = {'a' + (cmove.from & 7),
			'1' + (7 - (cmove.from >> 4)),
			'a' + (cmove.to & 7),
			'1' + (7 - (cmove.to >> 4))
	};

	if(cmove.promo != EMP) {
		if(cmove.promo > 5) cmove.promo = (Piece)(cmove.promo-6);
		r.push_back(str2[cmove.promo]);
	}

	return r;
}

extern u64 zobrist[128][13];
extern u64 zob_c;
extern gp_hash_table<u64, u64> ztable;
extern int wkpos, bkpos;
extern int perft_mm;
extern u8 castling;
CMove IND = {0,0,EMP,EMP,0};
// extern int perft;
int main() {
	build_board();
	printb();
	// auto t0 = chrono::high_resolution_clock::now();
	// // cout << minimax(0, 1)[1] << endl;
	// cout << perft(0, 1) << endl;
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
			mv = 1;
			build_board();
		}

		else if(inp.substr(0, 23) == "position startpos moves") {
			mv = 1;
			build_board();
			string u = inp;
			u.push_back(' ');
			vector<string> gg;
			string cr = "";
			for(int i = 23 ; i < u.size() ; i++) {
				if(u[i] == ' ') {
					if(cr.size()) gg.push_back(cr);
					cr = "";
					continue;
				}

				cr.push_back(u[i]);
			}

			for(int i = 0 ; i < gg.size() ; i++) {
				movegen(mv);
				bool fnd = 0;
				for(int j = 0 ; j < mvs ; j++) {
					string c = conv(Moves[j]);
					// cout << c << " " << gg[i] << endl;
					if(c == gg[i]) {
						// cout << "uwu" << endl;
						domove(Moves[j], 1);
						mv ^= 1;
						fnd = 1;
						break;
					}
				}

				if(!fnd) {
					cout << gg[i] << " " << i << " fix me" << endl;
					exit(0);
				}
			}

			cout << castling << endl;
			printb();

			// cout << "OMG " << ztable[zob_c] << endl;
		}

		else if(inp.substr(0, 2) == "go") {
			perft_mm = 0;
			// IND = -1;
			auto t0 = chrono::high_resolution_clock::now();
			minimax(0, mv);
			auto t1 = chrono::high_resolution_clock::now();
			movegen(mv);
			// cout << IND << endl;
			cout << "bestmove " << conv(IND) << endl;
			cout << perft_mm << " " << chrono::duration<double>(t1 - t0).count() << endl;
			domove(IND, 1);
			cout << castling << endl;
			printb();
			mv ^= 1;

			// cout << perft << " " << mvs << " " << ind << " OMG " << ztable[zob_c] << endl;
		}
	}
}