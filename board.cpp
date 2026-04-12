// board.cpp: everything that has to do with the board
#include "header.hpp"
BBT Board; // Global board

// zobrist
u64 zobrist[128][13];
const u64 zobrist_castling[16] = {17655471254146881536ULL, 18279671532425267200ULL, 2541236375155531264ULL,
                                978525948882968960ULL, 10420957505803663360ULL, 4062041258912541184ULL,
                                589856494314211456ULL, 10771295606568798208ULL, 5664616889478702080ULL,
                                5740304894085401600ULL, 8657072065486682112ULL, 9803964329434722304ULL,
                                8085386955865525248ULL, 15149151432984141824ULL, 7475191094976130048ULL, 2920194565133871104ULL};

const u64 zobrist_turn[2] = {9705194964410038272ULL, 7466648264651373568ULL};
u64 zobrist_enp[128];
u64 zob_c = 0;
gp_hash_table<u64, u64> ztable;
int wkpos, bkpos;

// movegen
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

CMove Moves[6001];
int mvs = 0;
int rays_s[6] = {0, 8, 4, 4, 8, 8}; // size, slight optimization

// rollback
u8 castling = 15;
CMove rollback[6001]; // claim: chess game can't be longer than 6001 moves, i added 1 to sound correct :pray:
u8 rollback_c[6001]; // for castling
CMove lstmv[6001]; // last move

int rb_p = 0; // pointer!
int lstmv_p = 0;
bool color(int i) {
    return Board[i] < 6;
}

bool samecolor(int i, int j) {
    return color(i) == color(j);
}

void printb() {
    string tns = "pnbrqkPNBRQK ";
    // array<char8_t[4], 12> tns = {
    //     u8"\u2659", u8"\u2658", u8"\u2657", u8"\u2656", u8"\u2655", u8"\u2654",
    //     u8"\u265F", u8"\u265E", u8"\u265D", u8"\u265C", u8"\u265B", u8"\u265A",
    // };

    // bool u = 0;
    // for(int i = 0 ; i < 8 ; i++) {
    //     for(int j = 0 ; j < 8 ; j++) {
    //         int k = 16*i + j;
    //         if(Board[k] == EMP) cout << "#";
    //         // else cout << reinterpret_cast<const char*>(tns[Board[k]]);
    //         else cout << tns[Board[k]];
    //     }

    //     cout << endl;
    // }

    bool u = 0;
    for(int i = 0 ; i < 8 ; i++) {
        for(int j = 0 ; j < 16 ; j++) {
            if(Board[16*i + j] == EMP) cout << "#";
            // else cout << reinterpret_cast<const char*>(tns[Board[k]]);
            else cout << tns[Board[16*i + j]];
        }

        cout << endl;
    }
}

void zob(int i) {
    zob_c ^= zobrist[i][Board[i]];
}

// three rep into trans table
u64 upd(bool turn) {
    int flag;
    if(rb_p>0) flag = lstmv[rb_p-1].to;
    else flag = 0;
    return zob_c ^ zobrist_castling[castling] ^ zobrist_turn[turn] ^ zobrist_enp[flag];
}

void build_zob() {
    zob_c = 0;
    mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
    for(int i = 0 ; i < 128 ; i++) {
        zobrist_enp[i] = rng();
        for(int j = 0 ; j < 13 ; j++) zobrist[i][j] = rng();
    }
}

extern float acc_w[HL1_SIZE];
extern float acc_b[HL1_SIZE];
extern float hidden1_w[INPUT_SIZE][HL1_SIZE];
extern float hidden1_b[HL1_SIZE];
int calc_nnue_index(int pos, bool color_view) {
    Piece pc = Board[pos];
    int sq = 8*(pos >> 4) + (pos&7);
    
    // black;
    bool side = pc>5;
    if(color_view == 0) {
        // pc ^= 1;
        sq ^= 0x70; // flipping for 0x88
    }

    return side*64*6 + 64 * (pc - side*6) + sq;
}

// resetting the accumulators (both of them)
void reset_acc() {
    int inputw[INPUT_SIZE];
    int inputb[INPUT_SIZE];
    memset(inputw, 0, sizeof inputw);
    memset(inputb, 0, sizeof inputb);

    for(int i = 0 ; i < 128 ; i++) {
        if(Board[i] == EMP or i&0x88) continue;
        inputb[calc_nnue_index(i, 0)] = 1;
        inputb[calc_nnue_index(i, 1)] = 1;
    }

    // memset(acc_w, 0, sizeof acc_w);
    for(int i = 0 ; i < HL1_SIZE ; i++) acc_w[i] = 0.f;
    for(int i = 0 ; i < HL1_SIZE ; i++) acc_b[i] = 0.f;
    for(int i = 0 ; i < HL1_SIZE ; i++) {
        float sw = 0.f, sb = 0.f;
        for(int j = 0 ; j < INPUT_SIZE ; j++) {
            sw += hidden1_w[j][i] * inputw[j];
            sb += hidden1_w[j][i] * inputb[j];
        }

        acc_w[i] = sw;
        acc_b[i] = sb;
    }
}

void build_board() {
    mvs = 0;
    castling = 0xf;
    rb_p = 0;
    lstmv_p = 0;
    zob_c = 0;
    ztable.clear();
    for(int i = 0 ; i < 128 ; i++) {
        Board[i] = EMP;
    }

    for(int i = 16 ; i < 24 ; i++) Board[i] = BP;
    for(int i = 96 ; i < 104 ; i++) Board[i] = WP;

    Board[0] = BR;
    Board[1] = BN;
    Board[2] = BB;
    Board[3] = BQ;
    Board[4] = BK;
    Board[5] = BB;
    Board[6] = BN;
    Board[7] = BR;
    Board[112] = WR;
    Board[113] = WN;
    Board[114] = WB;
    Board[115] = WQ;
    Board[116] = WK;
    Board[117] = WB;
    Board[118] = WN;
    Board[119] = WR;

    wkpos = 116;
    bkpos = 4;

    // zobrist
    for(int i = 0 ; i < 128 ; i++) {
        if(i&0x88) continue;
        zob(i);
    }

    // zob_c ^= zobrist_castle[15];
    ztable[zob_c]++;
    reset_acc();
}

void build_fen(string str) {
    mvs = 0;
    castling = 0xf;
    rb_p = 0;
    lstmv_p = 0;
    zob_c = 0;
    ztable.clear();
    for(int i = 0 ; i < 128 ; i++) {
        Board[i] = EMP;
    }

    int u = 0, i = 0, c = 1;
    for(; i < str.size() ; i++) {
        if(str[i] == ' ') break;
        if(str[i] == '/') {
            u = c << 4;
            c++;
            continue;
        }

        if('0' <= str[i] and str[i] <= '9') {
            u += str[i] - '0';
            continue;
        }

        // cout << u << endl;
        if(str[i] == 'p') Board[u] = BP;
        else if(str[i] == 'n') Board[u] = BN;
        else if(str[i] == 'b') Board[u] = BB;
        else if(str[i] == 'r') Board[u] = BR;
        else if(str[i] == 'q') Board[u] = BQ;
        else if(str[i] == 'k') Board[u] = BK;
        else if(str[i] == 'P') Board[u] = WP;
        else if(str[i] == 'N') Board[u] = WN;
        else if(str[i] == 'B') Board[u] = WB;
        else if(str[i] == 'R') Board[u] = WR;
        else if(str[i] == 'Q') Board[u] = WQ;
        else if(str[i] == 'K') Board[u] = WK;
        u++;
    }

    while(i < str.size() and str[i] == ' ') i++;
    if(i < str.size()) {
        extern bool mv;
        if(str[i] == 'w') mv = 1;
        else mv = 0;
    }

    while(i < str.size() and str[i] != ' ') i++;
    while(i < str.size() and str[i] == ' ') i++;

    castling = 0;
    if(i < str.size()) {
        if(str[i] == '-') while(i < str.size() and str[i] != ' ') i++;
        else {
            for(; i < str.size() and str[i] != ' '; i++) {
                if(str[i] == 'K') castling |= 1;
                else if(str[i] == 'Q') castling |= 2;
                else if(str[i] == 'k') castling |= 4;
                else if(str[i] == 'q') castling |= 8;
            }
        }
    }

    while(i < str.size() and str[i] == ' ') i++;
    // finish en-passant, ima sleep now
    lstmv_p = 0;
    if(i < str.size()) {
        if(str[i] != '-') {
            // two chars
            if(i + 1 < str.size()) {
                int f = str[i] - 'a';
                int fr = str[i+1] - '0';
                int r = 8 - fr;
                int ep = (r << 4) | f;

                extern bool mv;
                CMove Lm = {0,0,EMP,EMP,0};
                if(mv) {
                    int from = ep - 2 * pdir[0];
                    if(!(from & 0x88) and !(ep & 0x88)) {
                        Lm.from = (u8)from;
                        Lm.to   = (u8)ep;
                        Lm.capture = Board[ep];
                        Lm.promo = EMP;
                        Lm.flag = 0;
                        lstmv[0] = Lm;
                        lstmv_p = 1;
                    }
                }

                else {
                    int from = ep - 2 * pdir[1];
                    if(!(from & 0x88) and !(ep & 0x88)) {
                        Lm.from = (u8)from;
                        Lm.to   = (u8)ep;
                        Lm.capture = Board[ep];
                        Lm.promo = EMP;
                        Lm.flag = 0;
                        lstmv[0] = Lm;
                        lstmv_p = 1;
                    }
                }
            }

            while(i < str.size() and str[i] != ' ') i++;
        }

        else while(i < str.size() and str[i] != ' ') i++;
    }

    // update zobrist and wkpos bkpos
    zob_c = 0;
    wkpos = bkpos = -1;
    for(int i = 0 ; i < 128 ; i++) {
        if(i&0x88) continue;
        if(Board[i] == BK) bkpos = i;
        if(Board[i] == WK) wkpos = i;
        zob(i);
    }

    ztable[zob_c]++;
    reset_acc();
}

bool expincheck(bool kc, bool u) {
    int i;
    if(kc) i = wkpos;
    else i = bkpos;
    int k = 6*kc;
    // i see you, you see me
    for(int p = 1 ; p < 6 ; p++) {
        for(int j = 0 ; j < rays_s[p] ; j++) {
            u8 cur = i;
            for(;;) {
                cur += ray[p][j];
                if(cur & 0x88) break; // outside the board
                // cout << cur << endl;
                if(Board[cur] != EMP) {
                    if(Board[cur] == p + k) {
                        if(u) cout << p << endl;
                        return 1;
                    }

                    break;
                }
            
                if(!slide[p]) break; // not slidable
            }
        }
    }

    // pawns
    u8 p1 = i + pdir[kc] - 1;
    u8 p2 = i + pdir[kc] + 1;
    if(!(p1&0x88) and Board[p1] == k) {
        if(u) cout << 0 << endl;
        return 1;
    }

    if(!(p2&0x88) and Board[p2] == k) { 
        if(u) cout << 0 << endl;
        return 1;
    }

    return 0;
}

u64 bitboard = 0;
bool bincheck(int i) {
    int r = i >> 4;
    int f = i & 7;
    int u = 8*r + f;
    return bitboard & (1LL << u);
}

bool incheck(bool turn) {
    if(turn) return bincheck(wkpos);
    return bincheck(bkpos);
}

void add_bitboard(int i) {
    if(i&0x88) return;
    int r = i >> 4;
    int f = i & 7;
    int u = 8*r + f;
    bitboard |= (1LL << u);
}

void build_attack(bool mv) {
    bitboard = 0;
    CMove Lm;
    if(rb_p>0) Lm = lstmv[lstmv_p-1];
    else Lm = {0,0,EMP,EMP,0};
    for(int i = 0 ; i < 128 ; i++) {
        if(i&0x88) continue;
        Piece p = Board[i];
        if(color(i) != mv) continue;
        if(p == EMP) continue;
        int tp = p;
        if(tp > 5) tp -= 6;
        if(p == WP or p == BP) {
            // capturing
            int t = pdir[color(i)];
            u8 p1 = i + t + 1;
            u8 p2 = i + t - 1;
            add_bitboard(p1);
            add_bitboard(p2);
        }

        else for(int j = 0 ; j < rays_s[tp] ; j++) {
            u8 cur = i;
            // cout << "k" << p << " " << cur << " " << j << " " << rays_s[p] << endl;
            for(;;) {
                cur += ray[tp][j];
                if(cur & 0x88) break; // outside the board
                // cout << cur << endl;
                if(Board[cur] != EMP) {
                    if(color(i) != color(cur)) add_bitboard(cur); // capture
                    break;
                }
            
                add_bitboard(cur); // quiet move
                if(!slide[tp]) break; // not slidable
            }
        }
    }
}

void domove(CMove Move) {
    if(Move.from&0x88 or Move.to&0x88) {
        cout << Move.from << " " << Move.to << endl;
        cout << "BRO CMON" << endl;
        exit(0);
    }

    bool mvc = color(Move.from);
    rollback[rb_p] = Move;
    rollback_c[rb_p] = castling;
    rb_p++;

    lstmv[lstmv_p] = Move;
    lstmv_p++;
    // castling
    // abcd
    // ab: L/S BLACK, cd: L/S WHITE
    // zob_c ^= zobrist_castle[castling];
    if(Board[Move.from] == WR) {
        if((Move.from&7) == 0) SETZ(castling, 2);
        if((Move.from&7) == 7) SETZ(castling, 1);
    }

    else if(Board[Move.from] == BR) {
        if((Move.from&7) == 0) SETZ(castling, 8);
        if((Move.from&7) == 7) SETZ(castling, 4);
    }

    if(Board[Move.to] == WR) {
        if((Move.to&7) == 0) SETZ(castling, 2);
        if((Move.to&7) == 7) SETZ(castling, 1);
    }

    else if(Board[Move.to] == BR) {
        if((Move.to&7) == 0) SETZ(castling, 8);
        if((Move.to&7) == 7) SETZ(castling, 4);
    }


    if(Board[Move.from] == WK) {
        SETZ(castling, 3);
        wkpos = Move.to;
    }

    else if(Board[Move.from] == BK) {
        SETZ(castling, 12);
        bkpos = Move.to;
    }

    // zob_c ^= 4[castling];
    zob(Move.from);
    zob(Move.to);
    // zob_c ^= zobrist[Move.from][Board[Move.from]];
    // zob_c ^= zobrist[Move.to][Board[Move.to]];

    // pawn promo
    if(Move.promo != EMP) Board[Move.to] = Move.promo;
    else Board[Move.to] = Board[Move.from];
    Board[Move.from] = EMP;

    zob(Move.from);
    zob(Move.to);
    // zob_c ^= zobrist[Move.to][Board[Move.to]];
    // zob_c ^= zobrist[Move.from][Board[Move.from]];

    // en passant
    if(Move.flag == 1) {
        int cs = Move.to + pdir[!mvc];
        // if(roll) rollback[rb_p-1].capture = Board[cs];
        zob(cs);
        Board[cs] = EMP;
        zob(cs);
    }

    // short castling
    if(Move.flag == 2) {
        Piece m = Board[Move.to+1];
        zob(Move.to+1);
        zob(Move.to-1);
        Board[Move.to+1] = EMP;
        Board[Move.to-1] = m;
        zob(Move.to+1);
        zob(Move.to-1);
    }

    // long castling
    if(Move.flag == 3) {
        Piece m = Board[Move.to-2];
        zob(Move.to-2);
        zob(Move.to+1);
        Board[Move.to-2] = EMP;
        Board[Move.to+1] = m;
        zob(Move.to-2);
        zob(Move.to+1);
    }

    ztable[zob_c]++;
}

void undomove() {
    // shouldn't happen
    if(!rb_p) {
        cout << "FIX ME!" << endl;
        exit(0);
    }

    rb_p--;
    lstmv_p--;
    CMove rb = rollback[rb_p];
    // zob_c ^= zobrist_castle[castling];
    castling = rollback_c[rb_p];
    // zob_c ^= zobrist_castle[castling];
    ztable[zob_c]--;

    if(Board[rb.to] == WK) wkpos = rb.from;
    else if(Board[rb.to] == BK) bkpos = rb.from;

    zob(rb.from);
    zob(rb.to);
    // zob_c ^= zobrist[rb.from][Board[rb.from]];
    // zob_c ^= zobrist[rb.to][Board[rb.to]];

    if(rb.promo != EMP) Board[rb.from] = (Piece)(6*(!color(rb.to)));
    else Board[rb.from] = Board[rb.to];
    Board[rb.to] = rb.capture;

    zob(rb.from);
    zob(rb.to);
    // zob_c ^= zobrist[rb.to][Board[rb.to]];
    // zob_c ^= zobrist[rb.from][Board[rb.from]];

    // en passant
    if(rb.flag == 1) {
        bool mvcol = color(rb.from);
        int cs = rb.to + pdir[!mvcol];
        zob(cs);
        Board[cs] = (Piece)(6*mvcol);
        zob(cs);
    }

    // short castling
    if(rb.flag == 2) {
        Piece m = Board[rb.to-1];
        zob(rb.to-1);
        zob(rb.to+1);
        Board[rb.to+1] = m;
        Board[rb.to-1] = EMP;
        zob(rb.to+1);
        zob(rb.to-1);
    }

    // long castling
    if(rb.flag == 3) {
        Piece m = Board[rb.to+1];
        zob(rb.to-2);
        zob(rb.to+1);
        Board[rb.to-2] = m;
        Board[rb.to+1] = EMP;
        zob(rb.to-2);
        zob(rb.to+1);
    }
}

bool baha;
void AddMove(int from, int to, int flag, Piece promo) {
    // cout << from << " " << to << endl;
    if(from&0x88 or to&0x88) return;
    // Moves[mvs] = {from, to, Board[to], promo, 0};
    CMove m = {from, to, Board[to], promo, flag};
    domove(m);
    if(!expincheck(baha, 0)) {
        Moves[mvs++] = m;
    }

    undomove();
}

void movegen(bool mv) {
    mvs = 0;
    baha = mv;
    CMove Lm;
    int kp;
    if(mv) kp = wkpos;
    else kp = bkpos;
    // check for castling (holy this is gonna take LONG)
    // short castle
    int k = 0;
    if(!mv) k = 2;
    bool sc = castling&(1 << k); // didnt move
    if(sc) sc &= (Board[kp+1] == EMP and Board[kp+2] == EMP); // empty section
    if(sc) sc &= !bincheck(kp) and !bincheck(kp+1) and !bincheck(kp+2); // no checks
    if(sc) {
        // cout << "SC" << endl;
        // printb();
        AddMove(kp, kp+2, 2, EMP);
    }

    // long castle
    k++;
    bool lc = castling&(1 << k);
    if(lc) lc &= (Board[kp-1] == EMP and Board[kp-2] == EMP and Board[kp-3] == EMP); // empty section
    if(lc) lc &= !bincheck(kp) and !bincheck(kp-1) and !bincheck(kp-2); // no checks
    if(lc) {
        // cout << "LC" << endl;
        // printb();
        AddMove(kp, kp-2, 3, EMP);
    }

    if(rb_p>0) Lm = lstmv[lstmv_p-1];
    else Lm = {0,0,EMP,EMP,0};
    for(int i = 0 ; i < 128 ; i++) {
        if(i&0x88) continue;
        Piece p = Board[i];
        if(color(i) != mv) continue;
        if(p == EMP) continue;
        int tp = p;
        if(tp > 5) tp -= 6;
        if(p == WP or p == BP) {
            // capturing
            int t = pdir[color(i)];
            u8 p1 = i + t + 1;
            u8 p2 = i + t - 1;
            int r = i >> 4;
            bool promo = (color(i) and r == 1) or (!color(i) and r == 6);

            // capture
            if(Board[p1] != EMP and color(p1) != color(i)) {
                if(promo) for(int j = 1 ; j < 5 ; j++) AddMove(i, p1, 0, (Piece)(j+6*(!color(i))));
                else AddMove(i, p1, 0, EMP);
            }

            if(Board[p2] != EMP and color(p2) != color(i)) {
                if(promo) for(int j = 1 ; j < 5 ; j++) AddMove(i, p2, 0, (Piece)(j+6*(!color(i))));
                else AddMove(i, p2, 0, EMP);
            }

            // en passant
            // white taking
            if(Lm.to&0x88 or Lm.from&0x88) cout << Lm.from << " " << Lm.to << endl;
            if((Lm.from>>4) == 1 and (Lm.to>>4) == 3 and Board[Lm.to] == BP) {
                if(Lm.to == i+1) AddMove(i, p1, 1, EMP);
                else if(Lm.to == i-1) AddMove(i, p2, 1, EMP);
            }

            // black taking
            if((Lm.from>>4) == 6 and (Lm.to>>4) == 4 and Board[Lm.to] == WP) {
                if(Lm.to == i+1) AddMove(i, p1, 1, EMP);
                else if(Lm.to == i-1) AddMove(i, p2, 1, EMP);
            }

            // moves
            if(((i+t) & 0x88) or Board[i + t] != EMP) continue;
            if(promo) {
                for(int j = 1 ; j < 5 ; j++) AddMove(i, i + t, 0, (Piece)(j + 6*(!color(i))));
            } else AddMove(i, i + t, 0, EMP);

            if(((i+2*t) & 0x88) or Board[i + 2*t] != EMP) continue;
            if((!color(i) and r == 1) or (color(i) and r == 6)) AddMove(i, i + 2*t, 0, EMP);
        }

        else for(int j = 0 ; j < rays_s[tp] ; j++) {
            u8 cur = i;
            // cout << "k" << p << " " << cur << " " << j << " " << rays_s[p] << endl;
            for(;;) {
                cur += ray[tp][j];
                if(cur & 0x88) break; // outside the board
                // cout << cur << endl;
                if(Board[cur] != EMP) {
                    if(color(i) != color(cur)) AddMove(i, cur, 0, EMP); // capture
                    break;
                }
            
                AddMove(i, cur, 0, EMP); // quiet move
                if(!slide[tp]) break; // not slidable
            }
        }
    }
}