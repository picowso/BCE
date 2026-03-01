// board.cpp: everything that has to do with the board
#include "header.hpp"
BBT Board; // Global board
u8 color[128]; // 0: black, 1: white, 2: nothing
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
    Board[3] = K;
    Board[4] = Q;
    Board[5] = B;
    Board[6] = N;
    Board[7] = R;
    for(int i = 0 ; i < 8 ; i++) Board[i+112] = Board[i];
}

CMove rollback[DEPTH_LIMIT];
int rb_p = 0; // pointer!
void domove(CMove Move, bool roll) {
    if(roll) {
        rollback[rb_p] = Move;
        rb_p++;
    }

    color[Move.to] = color[Move.from];
    color[Move.from] = 2;
    Board[Move.to] = Board[Move.from];
    Board[Move.from] = EMP;
}

void undomove() {
    // shouldn't happen
    if(!rb_p) {
        cout << "FIX ME!" << endl;
        exit(0);
    }

    rb_p--;
    color[rollback[rb_p].from] = color[rollback[rb_p].to];
    if(rollback[rb_p].capture != EMP) color[rollback[rb_p].to] ^= 1;
    else color[rollback[rb_p].to] = 2;
    Board[rollback[rb_p].from] = Board[rollback[rb_p].to];
    Board[rollback[rb_p].to] = rollback[rb_p].capture;
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
    cout << from << " " << to << endl;
    if(from&0x88 or to&0x88) return;
    Moves[mvs] = {from, to, Board[to], EMP, 0};
    mvs++;
}

extern bool mv;
// TODO: en passant, castling
void movegen() {
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
            if(Board[p1] != EMP and color[p1] != color[i]) AddMove(i, p1);
            if(Board[p2] != EMP and color[p2] != color[i]) AddMove(i, p2);

            // moves
            if(!((i+t) & 0x88) and Board[i + t] != EMP) continue;
            AddMove(i, i + t);

            if(!((i+2*t) & 0x88) and Board[i + 2*t] != EMP) continue;
            AddMove(i, i + 2*t);
            
            // en passant
            continue;
        }

        else for(int j = 0 ; j < rays_s[p] ; j++) {
            int cur = i;
            // cout << "k" << p << " " << cur << " " << j << " " << rays_s[p] << endl;
            for(;;) {
                cur += ray[p][j];
                if(cur & 0x88) break; // outside the board
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