// board.cpp: everything that has to do with the board
#include "header.hpp"
BBT Board; // Global board
u8 color[128]; // 0: black, 1: white, 2: nothing

// zobrist
u64 zobrist[128][7][3];
u64 zob_c = 0;
gp_hash_table<u64, u64> ztable;
void build_board() {
    for(int i = 0 ; i < 128 ; i++) {
        Board[i] = EMP;
        color[i] = 2;
    }

    for(int i = 96 ; i < 128 ; i++) color[i] = 1;
    for(int i = 0 ; i < 32 ; i++) color[i] = 0;
    for(int i = 16 ; i < 24 ; i++) Board[i] = P;
    for(int i = 96 ; i < 112 ; i++) Board[i] = P;

    Board[0] = R;
    Board[1] = N;
    Board[2] = B;
    Board[3] = Q;
    Board[4] = K;
    Board[5] = B;
    Board[6] = N;
    Board[7] = R;
    for(int i = 0 ; i < 8 ; i++) Board[i+112] = Board[i];

    // zobrist
    zob_c = 0;
    mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
    for(int i = 0 ; i < 128 ; i++) {
        for(int j = 0 ; j < 7 ; j++) {
            for(int k = 0 ; k < 3 ; k++) {
                zobrist[i][j][k] = rng();
            }
        }
    }

    for(int i = 0 ; i < 128 ; i++) {
        if(i&0x88) continue;
        zob_c ^= zobrist[i][Board[i]][color[i]];
    }

    ztable[zob_c]++;
}

void printb() {
    string tns = "pnbrqkPNBRQK";
    for(int i = 0 ; i < 8 ; i++) {
        for(int j = 0 ; j < 8 ; j++) {
            int l = 16*i + j;
            if(Board[l] == EMP) cout << " ";
            else if(color[l]) cout << tns[Board[l] + 6];
            else cout << tns[Board[l]];
        }

        cout << endl;
    }
}

CMove rollback[DEPTH_LIMIT+1];
u8 rb_c[DEPTH_LIMIT+1];
int rb_p = 0; // pointer!
void domove(CMove Move, bool roll) {
    if(Move.from&0x88 or Move.to&0x88) {
        cout << Move.from << " " << Move.to << endl;
        cout << "BRO" << endl;
        // exit(0);
    }

    if(roll) {
        rollback[rb_p] = Move;
        rb_c[rb_p] = color[Move.to];
        rb_p++;
    }

    zob_c ^= zobrist[Move.from][Board[Move.from]][color[Move.from]];
    zob_c ^= zobrist[Move.to][Board[Move.to]][color[Move.to]];
    Board[Move.to] = Board[Move.from];
    Board[Move.from] = EMP;
    color[Move.to] = color[Move.from];
    color[Move.from] = 2;
    zob_c ^= zobrist[Move.to][Board[Move.to]][color[Move.to]];
    zob_c ^= zobrist[Move.from][Board[Move.from]][color[Move.from]];
    ztable[zob_c]++;
}

// the source of my misery:
void undomove() {
    // shouldn't happen
    if(!rb_p) {
        cout << "FIX ME!" << endl;
        exit(0);
    }

    rb_p--;
    CMove rb = rollback[rb_p];
    // color[rollback[rb_p].from] = color[rollback[rb_p].to];
    // if(rollback[rb_p].capture != EMP) color[rollback[rb_p].to] ^= 1;
    // else color[rollback[rb_p].to] = 2;
    // Board[rollback[rb_p].from] = Board[rollback[rb_p].to];
    // Board[rollback[rb_p].to] = rollback[rb_p].capture;
    ztable[zob_c]--;
    zob_c ^= zobrist[rb.from][Board[rb.from]][color[rb.from]];
    zob_c ^= zobrist[rb.to][Board[rb.to]][color[rb.to]];
    Board[rb.from] = Board[rb.to];
    Board[rb.to] = rb.capture;
    color[rb.from] = color[rb.to];
    color[rb.to] = rb_c[rb_p];
    zob_c ^= zobrist[rb.to][Board[rb.to]][color[rb.to]];
    zob_c ^= zobrist[rb.from][Board[rb.from]][color[rb.from]];
}

int dirs[8] = {1, -1, 16, -16, 17, 15, -15, -17};
int pdir[2] = {16, -16};
bool slide[6] = {0, 0, 1, 1, 1, 0};
int ray[6][8] = {
    {  0,   0,  0,  0, 0,  0,  0,  0 },
    { 33, 31, 18, 14, -14, -18, -31, -33 }, // knight
    { 17, -17, 15, -15, 0, 0, 0, 0 }, // bishop
    { -16,  -1,  1, 16, 0,  0,  0,  0 }, // rook
    { 1, -1, 16, -16, 17, 15, -15, -17}, // queen
    { 1, -1, 16, -16, 17, 15, -15, -17}  // king
};

CMove Moves[218]; // https://lichess.org/@/Tobs40/blog/why-a-reachable-position-can-have-at-most-218-playable-moves/a5xdxeqs
int mvs = 0;
bool side; // which side's turn
int rays_s[6] = {0, 8, 4, 4, 8, 8}; // size, slight optimization
void AddMove(int from, int to) {
    // cout << from << " " << to << endl;
    if(from&0x88 or to&0x88) return;
    Moves[mvs] = {from, to, Board[to], EMP, 0};
    mvs++;
}

// TODO: en passant, castling
void movegen(bool mv) {
    mvs = 0;
    for(int i = 0 ; i < 128 ; i++) {
        if(i&0x88) continue;
        Piece p = Board[i];
        // cout << "G" << mv << " " << color[i] << " " << i << endl;
        if(color[i] != mv) continue;
        if(p == EMP) continue;
        if(p == P) {
            // capturing
            int t = pdir[color[i]];
            int p1 = i + t + 1;
            int p2 = i + t - 1;
            int r = i >> 4;
            if(Board[p1] != EMP and color[p1] != color[i]) AddMove(i, p1);
            if(Board[p2] != EMP and color[p2] != color[i]) AddMove(i, p2);

            // moves
            if(((i+t) & 0x88) or Board[i + t] != EMP) continue;
            AddMove(i, i + t);

            if(((i+2*t) & 0x88) or Board[i + 2*t] != EMP) continue;
            if((!color[i] and r == 1) or (color[i] and r == 6)) AddMove(i, i + 2*t);
            
            // en passant
            continue;
        }

        else for(int j = 0 ; j < rays_s[p] ; j++) {
            int cur = i;
            // cout << "k" << p << " " << cur << " " << j << " " << rays_s[p] << endl;
            for(;;) {
                cur += ray[p][j];
                if(cur & 0x88) break; // outside the board
                // cout << cur << endl;
                if(Board[cur] != EMP) {
                    if(color[i] != color[cur]) AddMove(i, cur); // capture
                    break;
                }
            
                AddMove(i, cur); // quiet move
                if(!slide[p]) break; // not slidable
            }
        }
    }
}