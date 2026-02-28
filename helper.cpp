#include "header.hpp"
extern vector<move> Moves;
extern vector<move> EPMv;
extern stack<vector<fmov>> fmoves;
extern pos KPoses[2];
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
        // for(int j = 0 ; j < 8 ; j++) cout << tns[Board[i][j].type];
        cout << endl;
    }
}

void build_board(BBT &Board) {
    // building the board
    for(int i = 0 ; i < 8 ; i++) {
        for(int j = 0 ; j < 8 ; j++) Board[i][j] = EMP;
    }

    for (int i = 0 ; i < 8 ; i++) {
        Board[1][i] = {BPawn, 0};
        Board[6][i] = {WPawn, 0};
    }

    Board[0][0] = Board[0][7] = {BRook, 0};
    Board[0][1] = Board[0][6] = {BKnight, 0};
    Board[0][2] = Board[0][5] = {BBishop, 0};
    Board[0][3] = {BQueen, 0};
    Board[0][4] = {BKing, 0};

    Board[7][0] = Board[7][7] = {WRook, 0};
    Board[7][1] = Board[7][6] = {WKnight, 0};
    Board[7][2] = Board[7][5] = {WBishop, 0};
    Board[7][3] = {WQueen, 0};
    Board[7][4] = {WKing, 0};
}

bool incheck(BBT &Board, int mvnm) {
    movegen(Board, mvnm+1);
    pos kg = KPoses[mvnm&1];
    // for(int j = 0 ; j < 8 ; j++) {
    //     for(int k = 0 ; k < 8 ; k++) {
    //         if(Board[j][k].type%6 == 5 and (Board[j][k].type < 6) == (mvnm&1)) {
    //             kg = {j, k};
    //         }
    //     }
    // }

    bool pos = 0;
    for(move pl: Moves) {
        if(pl.S == kg) {
            pos = 1;
        }
    }

    return pos;
}

void movegen(BBT &Board, int movn) {
    Moves.clear();
    EPMv.clear();
    bool color = movn&1;
    for(int i = 0 ; i < 8 ; i++) {
        for(int j = 0 ; j < 8 ; j++) {
            pos curr = {i, j};
            Piece cur = Board[i][j];
            int tp = cur.type;
            if(tp > 5) tp -= 6;
            int clr = -1;
            if(color) clr *= -1;
            if(cur.type == VIDE) continue;
            if(cur.type/6 != color) continue;
            if(tp == 0) {
                // 1-move
                pos lcur = curr;
                lcur.F += clr;
                if(good(Board, curr, lcur)) Moves.push_back({curr, lcur});
                // 2-move
                if(Board[curr.F][curr.S].lstm == 0) {
                    bool cl = good(Board, curr, lcur);
                    lcur.F += clr;
                    if(cl and good(Board, curr, lcur)) Moves.push_back({curr, lcur});
                }

                // take
                vector<pos> gg = {{clr,1}, {clr,-1}};
                for(pos k: gg) {
                    pos kcur = curr;
                    add(kcur, k);
                    if(take(Board, curr, kcur)) Moves.push_back({curr, kcur});
                }

                // en passant
                vector<pos> ngg = {{clr,1}, {clr,-1}};
                for(pos k: ngg) {
                    pos kcur = curr;
                    add(kcur, k);
                    if(good(Board, curr, kcur) and (Board[kcur.F-clr][kcur.S].type == 0 or Board[kcur.F-clr][kcur.S].type == 6)
                        and Board[kcur.F-clr][kcur.S].lstm == movn-1
                        and !samecolor(Board, curr, {kcur.F-clr, kcur.S})
                        and curr.F == 3+color) {
                            EPMv.push_back({curr, kcur});
                    }
                }
            }

            // knight
            if(tp == 1) {
                for(int k = 0 ; k < 4 ; k++) {
                    vector<pos> gg = {{2,1}, {1,2}};
                    if(k&1) {
                        gg[0].F *= -1;
                        gg[1].F *= -1;
                    }

                    if(k&2) {
                        gg[0].S *= -1;
                        gg[1].S *= -1;
                    }

                    for(int l = 0 ; l < 2 ; l++) {
                        gg[l].F += i;
                        gg[l].S += j;
                    }

                    if(tng(Board, curr, gg[0])) Moves.push_back({curr, gg[0]});
                    if(tng(Board, curr, gg[1])) Moves.push_back({curr, gg[1]});
                }
            }

            // bishop
            if(tp == 2) {
                for(int k = 0 ; k < 4 ; k++) {
                    pos gg = {1,1};
                    if(k&1) gg.F *= -1;
                    if(k&2) gg.S *= -1;
                    pos tcur = curr;
                    add(tcur, gg);
                    while(inboard(tcur) and Board[tcur.F][tcur.S].type == VIDE) {
                        Moves.push_back({curr, tcur});
                        add(tcur, gg);
                    }

                    if(take(Board, curr, tcur)) Moves.push_back({curr, tcur});
                }
            }

            // rook
            if(tp == 3) {
                vector<pos> gg = {{0,1}, {1,0}, {-1,0}, {0,-1}};
                for(pos k: gg) {
                    pos tcur = curr;
                    add(tcur, k);
                    while(inboard(tcur) and Board[tcur.F][tcur.S].type == VIDE) {
                        Moves.push_back({curr, tcur});
                        add(tcur, k);
                    }

                    if(take(Board, curr, tcur)) Moves.push_back({curr, tcur});
                }
            }

            // queen
            if(tp == 4) {
                vector<pos> gg = {{0,1}, {1,0}, {-1,0}, {0,-1}};
                for(pos k: gg) {
                    pos tcur = curr;
                    add(tcur, k);
                    while(inboard(tcur) and Board[tcur.F][tcur.S].type == VIDE) {
                        Moves.push_back({curr, tcur});
                        add(tcur, k);
                    }

                    if(take(Board, curr, tcur)) Moves.push_back({curr, tcur});
                }

                for(int k = 0 ; k < 4 ; k++) {
                    pos gg = {1,1};
                    if(k&1) gg.F *= -1;
                    if(k&2) gg.S *= -1;
                    pos tcur = curr;
                    add(tcur, gg);
                    while(inboard(tcur) and Board[tcur.F][tcur.S].type == VIDE) {
                        Moves.push_back({curr, tcur});
                        add(tcur, gg);
                    }

                    if(take(Board, curr, tcur)) Moves.push_back({curr, tcur});
                }
            }

            // king
            if(tp == 5) {
                vector<pos> gg = {{0,1}, {1,0}, {-1,0}, {0,-1}, {1,1}, {-1,1}, {1,-1}, {-1,-1}};
                for(pos k: gg) {
                    pos lcur = {i,j};
                    add(lcur, k);
                    if(tng(Board, curr, lcur)) Moves.push_back({curr, lcur});
                }
            }
        }
    }
}

// generates all moves that can be rollbacked
vector<vector<fmov>> bmovesgen(BBT &Board, int i) {
    vector<vector<fmov>> res;
    movegen(Board, i);
    int kj = Moves.size();
    vector<move> fMoves = Moves;
    for(move k: EPMv) fMoves.push_back(k);
    
    // Short castle
    movegen(Board, i+1);
    bool u = 1;
    uchar rank = 7*(!(i&1));
    for(move k: Moves) {
        if(k.S.F == rank and 4 <= k.S.S and k.S.S <= 6) u = 0;
    }

    bool w = 1;
    for(int j = 5 ; j <= 6 ; j++) {
        if(Board[rank][j].type != VIDE) w = 0;
    }

    w = w and max(Board[rank][4].lstm, Board[rank][7].lstm) == 0;
    if(w and u) {
        vector<fmov> rr;
        rr.push_back({rank, (uchar)4, EMP});
        rr.push_back({rank, 7, EMP});
        rr.push_back({rank, 5, Board[rank][7]});
        rr.push_back({rank, 6, Board[rank][4]});
        res.push_back(rr);
    }

    // Long Castle
    u = 1;
    rank = 7*(!(i&1));
    for(move k: Moves) {
        if(k.S.F == rank and 2 <= k.S.S and k.S.S <= 4) u = 0;
    }

    w = 1;
    for(int j = 1 ; j <= 3 ; j++) {
        if(Board[rank][j].type != VIDE) w = 0;
    }

    w = w and max(Board[rank][0].lstm, Board[rank][4].lstm) == 0;
    if(w and u) {
        vector<fmov> rr;
        rr.push_back({rank, 0, EMP});
        rr.push_back({rank, 4, EMP});
        rr.push_back({rank, 2, Board[rank][4]});
        rr.push_back({rank, 3, Board[rank][0]});
        res.push_back(rr);
    }
    
    for(int j = 0 ; j < fMoves.size() ; j++) {
        move pl = fMoves[j];
        int clr = -1;
        if(i&1) clr *= -1;
        vector<fmov> rr;
        if(j >= kj) rr.push_back({pl.S.F-clr, pl.S.S, EMP});
        rr.push_back({pl.S.F, pl.S.S, Board[pl.F.F][pl.F.S]});
        rr.push_back({pl.F.F, pl.F.S, EMP});

        // king in check after the move?
        domove(Board, 1, rr);
        bool u = incheck(Board, i);
        undomove(Board);
        if(u) continue;

        // pawn promo
        int q = 7*(!(i&1));
        if(Board[pl.S.F][pl.S.S].type != VIDE and (Board[pl.S.F][pl.S.S].type == 0 or Board[pl.S.F][pl.S.S].type == 6) and pl.S.F == q) {
            for(int j = 1 ; j <= 4 ; j++) {
                Piece mh = Board[pl.S.F][pl.S.S];
                mh.type = (Type)(j + (!(i&1))*6);
                rr.push_back({pl.S.F, pl.S.S, mh});
                res.push_back(rr);
                rr.pop_back();
            }
        }

        else res.push_back(rr);
    }

    return res;
}

// generates moves in UCI notation:
// is this code repetition? yes, can we avoid it? not really
vector<string> ucimovesgen(BBT &Board, int i) {
    vector<string> res;
    movegen(Board, i);
    int kj = Moves.size();
    vector<move> fMoves = Moves;
    for(move k: EPMv) fMoves.push_back(k);
    
    // Short castle
    movegen(Board, i+1);
    bool u = 1;
    uchar rank = 7*(!(i&1));
    for(move k: Moves) {
        if(k.S.F == rank and 4 <= k.S.S and k.S.S <= 6) u = 0;
    }

    bool w = 1;
    for(int j = 5 ; j <= 6 ; j++) {
        if(Board[rank][j].type != VIDE) w = 0;
    }

    w = w and max(Board[rank][4].lstm, Board[rank][7].lstm) == 0;
    if(w and u) {
        string rr = "e g ";
        rr[1] = 7 - rank + '1';
        rr[3] = 7 - rank + '1';
        res.push_back(rr);
    }

    // Long Castle
    u = 1;
    rank = 7*(!(i&1));
    for(move k: Moves) {
        if(k.S.F == rank and 2 <= k.S.S and k.S.S <= 4) u = 0;
    }

    w = 1;
    for(int j = 1 ; j <= 3 ; j++) {
        if(Board[rank][j].type != VIDE) w = 0;
    }

    w = w and max(Board[rank][0].lstm, Board[rank][4].lstm) == 0;
    if(w and u) {
        string rr = "e c ";
        rr[1] = 7 - rank + '1';
        rr[3] = 7 - rank + '1';
        res.push_back(rr);
    }
    
    for(int j = 0 ; j < fMoves.size() ; j++) {
        move pl = fMoves[j];
        int clr = -1;
        if(i&1) clr *= -1;
        string rr = {pl.F.S + 'a', 7 - pl.F.F + '1', pl.S.S + 'a', 7 - pl.S.F + '1'};
        // cout << pl.F.F << " " << pl.F.S << " " << pl.S.F << " " << pl.S.S << " " << rr << endl;
        vector<fmov> rw;
        if(j >= kj) rw.push_back({pl.S.F-clr, pl.S.S, EMP});
        rw.push_back({pl.S.F, pl.S.S, Board[pl.F.F][pl.F.S]});
        rw.push_back({pl.F.F, pl.F.S, EMP});

        // king in check after the move?
        domove(Board, 1, rw);
        bool u = incheck(Board, i);
        undomove(Board);
        if(u) continue;

        // pawn promo
        int q = 7*(!(i&1));
        char ind[4] = {'n', 'b', 'r', 'q'};
        if(Board[pl.S.F][pl.S.S].type != VIDE and (Board[pl.S.F][pl.S.S].type == 0 or Board[pl.S.F][pl.S.S].type == 6) and pl.S.F == q) {
            for(int j = 0 ; j < 4 ; j++) {
                // Piece mh = Board[pl.S.F][pl.S.S];
                // mh.type = (Type)(j + (!(i&1))*6);
                rr.push_back(ind[j]);
                res.push_back(rr);
                rr.pop_back();
            }
        }

        else res.push_back(rr);
    }

    return res;
}

vector<BBT> boardgen(BBT &Board, int i) {
    // all *legal* boards from Board
    vector<BBT> res;
    vector<vector<fmov>> mvs = bmovesgen(Board, i);
    for(int j = 0 ; j < mvs.size() ; j++) {
        domove(Board, 1, mvs[j]);
        res.push_back(Board);
        undomove(Board);
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
        Board[a][b] = c;
        Board[a][b].lstm = movn;
    }

    if(onstack) fmoves.push(puh);
}

void undomove(BBT &Board) {
    // shouldn't happen
    if(fmoves.empty()) {
        cout << "STACK EMPTY\n";
        exit(0);
    }

    vector<fmov> puh = fmoves.top();
    fmoves.pop();
    // domove(Board, 0, puh); false we need reverse order
    for(int i = puh.size() - 1 ; i >= 0 ; i--) {
        short a = get<0>(puh[i]), b = get<1>(puh[i]);
        Piece c = get<2>(puh[i]);
        if(c.type == 5) KPoses[0] = {a, b};
        if(c.type == 11) KPoses[1] = {a, b};
        Board[a][b] = c;
    }
}