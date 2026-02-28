// helper.cpp: things that help other major parts
#include "header.hpp"
extern vector<move> Moves;
extern vector<move> EPMv;
extern stack<vector<fmov>> fmoves;
extern pos KPoses[2];
unordered_map<ull, short> hashes;
bool samecolor(BBT &Board, pos x, pos y) {
    int a = Board[x.F][x.S].type;
    int b = Board[y.F][y.S].type;
    return(a < 6 and b < 6) or (a > 5 and b > 5);
}

bool inboard(pos x) {
    return x.F >= 0 and x.F < 8 and x.S >= 0 and x.S < 8;
}

// is it alr to move from x to y
bool good(BBT &Board, pos x, pos y) {
    return inboard(y) and Board[y.F][y.S].type == VIDE;
}

bool take(BBT &Board, pos x, pos y) {
    return inboard(y) and !samecolor(Board, x, y) and Board[y.F][y.S].type != VIDE;
}

bool tng(BBT &Board, pos x, pos y) {
    return inboard(y) and (Board[y.F][y.S].type == VIDE or !samecolor(Board, x, y));
}

void add(pos &x, pos y) {
    x.F += y.F;
    x.S += y.S;
}

void displ(BBT Board) {
    // string tns = u8"♟♞♝♜♛♚♙♘♗♖♕♔ ";
    // string tns = "pmbrqkPMBRQK ";
    // cout << "\x1B[2J\x1B[H";
    array<char8_t[4], 13> tns = {
        u8"\u265F", u8"\u265E", u8"\u265D", u8"\u265C", u8"\u265B", u8"\u265A",
        u8"\u2659", u8"\u2658", u8"\u2657", u8"\u2656", u8"\u2655", u8"\u2654",  u8" "
    };

    for(int i = 0 ; i < 8 ; i++) {
        for(int j = 0 ; j < 8 ; j++) cout << reinterpret_cast<const char*>(tns[Board[i][j].type]);
        cout << endl;
    }
}


// hashes the board
// TODO: get a better hashtable, use https://en.wikipedia.org/wiki/Zobrist_hashing (make better random numbers.)
stack<ull> hashesst;
ull chash;
const ull MOD = 1e9 + 7;
ull binpowm(ull a, ull b, ull m) {
    ull res = 1;
    while(b > 0) {
        if(b&1) res = (res*a)%m;
        a = (a*a)%m;
        b >>= 1;
    }
    
    return res;
}

ull hashb(BBT &Board) {
    ull res = 0;
    ull cc = 1, p = 31;
    for(int i = 0; i < 8 ; i++) {
      for(int j = 0; j < 8 ; j++) {
        res += cc * (Board[i][j].type+1);
        cc = (cc*p)%MOD;
        res %= MOD;
      }
    }

    return res;
}

// domove/undomove:
// i'll keep a stack of last copy so that when i wanna undo i check top of the stack in the dfs/minimax
void domove(BBT &Board, bool onstack, vector<fmov> &mov, int movn) {
    vector<fmov> puh;
    for(int i = 0 ; i < mov.size() ; i++) {
        short a = get<0>(mov[i]), b = get<1>(mov[i]);
        Piece c = get<2>(mov[i]);
        puh.push_back({a, b, Board[a][b]});
        if(c.type == 5) KPoses[0] = {a, b};
        if(c.type == 11) KPoses[1] = {a, b};
        int ps = 8*a + b;
        chash = (chash - (Board[a][b].type+1) * binpowm(31, ps, MOD) + MOD)%MOD;
        Board[a][b] = c;
        chash = (chash + (Board[a][b].type+1) * binpowm(31, ps, MOD) + MOD)%MOD;
        Board[a][b].lstm = movn;
    }

    if(onstack) {
        hashes[chash]++;
        fmoves.push(puh);
        hashesst.push(chash);
        // return hash[hs] < 3;
    }

    // return 1;
}

void undomove(BBT &Board) {
    // shouldn't happen
    if(fmoves.empty()) {
        cout << "STACK EMPTY\n";
        exit(0);
    }

    vector<fmov> puh = fmoves.top();
    ull hsh = hashesst.top();
    hashesst.pop();
    fmoves.pop();
    hashes[hsh]--;
    for(int i = puh.size() - 1 ; i >= 0 ; i--) {
        short a = get<0>(puh[i]), b = get<1>(puh[i]);
        Piece c = get<2>(puh[i]);
        if(c.type == 5) KPoses[0] = {a, b};
        if(c.type == 11) KPoses[1] = {a, b};
        Board[a][b] = c;
    }
}