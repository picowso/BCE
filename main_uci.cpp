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

extern u8 color[128]; // 0: black, 1: white, 2: nothing
extern u64 zobrist[128][7][3];
extern u64 zob_c;
extern gp_hash_table<u64, u64> ztable;
extern int wkpos, bkpos;
extern int perft;
int main() {
	build_board();
	auto t0 = chrono::high_resolution_clock::now();
	cout << minimax(0, 1)[0] << endl;
	auto t1 = chrono::high_resolution_clock::now();
	cout << chrono::duration<double>(t1 - t0).count() << endl;
	cout << perft << endl;
	exit(0);
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
			cout << mvs << endl;
			for(int i = 0 ; i < mvs ; i++) {
				CMove Move = Moves[i];
				cout << conv(Move) << endl;
			// 	if(Move.from&0x88 or Move.to&0x88) {
			//         cout << Move.from << " " << Move.to << endl;
			//         cout << "BRO" << endl;
			//         exit(0);
			//     }

			//     if(Board[Move.from] == K) {
			//         if(color[Move.from]) wkpos = Move.to;
			//         else bkpos = Move.to;
			//     }

			//     Piece cap = Board[Move.to];
			//     u8 clr = color[Move.to];
			//     zob_c ^= zobrist[Move.from][Board[Move.from]][color[Move.from]];
			//     zob_c ^= zobrist[Move.to][Board[Move.to]][color[Move.to]];
			//     Board[Move.to] = Board[Move.from];
			//     Board[Move.from] = EMP;
			//     color[Move.to] = color[Move.from];
			//     color[Move.from] = 2;
			//     if(incheck(color[Move.to], 1)) {
			//     	cout << conv(Move) << endl;
			// 	}

		    //     Board[Move.from] = Board[Move.to];
		    //     Board[Move.to] = cap;
		    //     color[Move.from] = color[Move.to];
		    //     color[Move.to] = clr;
		    //     zob_c ^= zobrist[Move.from][Board[Move.from]][color[Move.from]];
		    //     zob_c ^= zobrist[Move.to][Board[Move.to]][color[Move.to]];
		    //     if(Board[Move.from] == K) {
		    //         if(color[Move.from]) wkpos = Move.from;
		    //         else bkpos = Move.from;
		    //     }
			}

			cout << "bestmove " << conv(Moves[ind]) << endl;
			domove(Moves[ind], 0);
			cout << mvs << " " << ind << " OMG " << ztable[zob_c] << endl;
		}
	}
}