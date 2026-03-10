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
	build_zob();
	build_board();
	// build_fromfen("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2 ");
	printb();
	movegen(!mv);
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

		else if(inp == "stop") {
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
			build_board();
			mv = 1;
			string u = "";
			vector<string> gg;
			inp.push_back(' ');
			for(int i = 23 ; i < inp.size() ; i++) {
				if(inp[i] == ' ') {
					if(u.size()) gg.push_back(u);
					u = "";
				}

				else u.push_back(inp[i]);
			}

			for(int i = 0 ; i < gg.size() ; i++) {
				bool fnd = 0;
				build_attack(!mv);
				movegen(mv);
				for(int j = 0 ; j < mvs ; j++) {
					// cout << conv(Moves[j]) << " " << gg[i] << endl;
					if(conv(Moves[j]) == gg[i]) {
						domove(Moves[j]);
						fnd = 1;
						mv ^= 1;
						break;
					}
				}

				if(!fnd) {
					cout << (int)castling << " BROOO" << endl;
					exit(0);
				}
			}

			cout << "OMG " << ztable[zob_c] << endl;
			printb();
			// cout << "OMG " << ztable[zob_c] << endl;
		}

		else if(inp.substr(0, 2) == "go") {
			int stab = 0;
			double u = 0.;
			string w = "";
			vector<string> gg;
			inp.push_back(' ');
			for(int i = 2 ; i < inp.size() ; i++) {
				if(inp[i] == ' ') {
					if(w.size()) gg.push_back(w);
					w = "";
				}

				else w.push_back(inp[i]);
			}

			map<string, string> uwu;
			for(int i = 0 ; i < gg.size() ; i += 2) {
				uwu[gg[i]] = gg[i+1];
			}

			int ttw = 5000;
			if(uwu.contains("wtime")) ttw = stoi(uwu["wtime"]);
			int ttb = 5000;
			if(uwu.contains("btime")) ttb = stoi(uwu["btime"]);
			// cout << tw << " " << tb << endl;
			if(mv) swap(ttb, ttw);
			double tw = ttw / 1000.;
			double tb = ttb / 1000.;

			// Iterative deepening + MTD(f)
			int firstguess = 10;
			for(int i = 1 ; i < DEPTH_MAX ; i++) {
				perft_mm = 0;
				IND = {0,0,EMP,EMP,0};
				CMove LIND = IND;
				auto t0 = chrono::high_resolution_clock::now();
				// firstguess = mtdf(mv, firstguess, i);
				firstguess = minimax(i, i, mv);
				auto t1 = chrono::high_resolution_clock::now();
				if(LIND.from != IND.from or LIND.to != IND.to) stab++;
				u += chrono::duration<double>(t1 - t0).count();
				cout << u << endl;
				if(17*u > min({4.5, stab + tb/2, tb})) {
					cout << i << endl;
					break;
				}
			}

			// cout << IND << endl;
			cout << "bestmove " << conv(IND) << endl;
			cout << mvs << endl;
			// cout << perft_mm << " " << chrono::duration<double>(t1 - t0).count() << endl;
			domove(IND);
			cout << castling << endl;
			printb();
			mv ^= 1;

			// cout << perft << " " << mvs << " " << ind << " OMG " << ztable[zob_c] << endl;
		}
	}
}