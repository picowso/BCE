#include "header.hpp"
// extern BBT Board;
extern vector<move> Moves;
extern vector<move> EPMv;

bool samecolor(BBT &Board, pos x, pos y) {
    int a = Board[x.F][x.S].type;
    int b = Board[y.F][y.S].type;
    return max(a, b) < 6 or min(a, b) > 5;
}

bool inboard(pos x) {
    return x.F >= 0 and x.F < 8 and x.S >= 0 and x.S < 8;
}

// is it alr to move from x to y
bool good(BBT &Board, pos x, pos y) {
    return inboard(x) and inboard(y) and Board[y.F][y.S].type == VIDE;
}

// bool take(pos x, pos y) {
bool take(BBT &Board, pos x, pos y) {
    return inboard(x) and inboard(y) and !samecolor(Board, x, y) and Board[y.F][y.S].type != VIDE;
}

// bool tng(pos x, pos y) {
bool tng(BBT &Board, pos x, pos y) {
    return inboard(x) and inboard(y) and (Board[y.F][y.S].type == VIDE or !samecolor(Board, x, y));
}

void add(pos &x, pos y) {
    x.F += y.F;
    x.S += y.S;
}

void displ(BBT Board) {
    cout << "\x1B[2J\x1B[H";
    // string tns = u8"♟♞♝♜♛♚♙♘♗♖♕♔ ";
    // u8string tns = u8"\u2654\u2655\u2656\u2657\u2658\u2659\u265A\u265B\u265C\u265D\u265E\u265F";
    // vector<wchar_t> tns(tnss.begin(), tnss.end());
    // string tns = "pmbrqkPMBRQK ";
    array<char8_t[4], 13> tns = {
        u8"\u265F", u8"\u265E", u8"\u265D", u8"\u265C", u8"\u265B", u8"\u265A",
        u8"\u2659", u8"\u2658", u8"\u2657", u8"\u2656", u8"\u2655", u8"\u2654",  u8" "
    };

    for(int i = 0 ; i < 8 ; i++) {
        for(int j = 0 ; j < 8 ; j++) cout << reinterpret_cast<const char*>(tns[Board[i][j].type]);
        cout << '\n';
    }
}

void build_board(BBT &Board) {
    // building the board
    for(int i = 0 ; i < 8 ; i++) {
        for(int j = 0 ; j < 8 ; j++) Board[i][j] = {VIDE, 0, -1};
    }

    for (int i = 0 ; i < 8 ; i++) {
        Board[1][i] = {BPawn, 0, -1};
        Board[6][i] = {WPawn, 0, -1};
    }

    Board[0][0] = Board[0][7] = {BRook, 0, -1};
    Board[0][1] = Board[0][6] = {BKnight, 0, -1};
    Board[0][2] = Board[0][5] = {BBishop, 0, -1};
    Board[0][3] = {BQueen, 0, -1};
    Board[0][4] = {BKing, 0, -1};

    Board[7][0] = Board[7][7] = {WRook, 0, -1};
    Board[7][1] = Board[7][6] = {WKnight, 0, -1};
    Board[7][2] = Board[7][5] = {WBishop, 0, -1};
    Board[7][3] = {WQueen, 0, -1};
    Board[7][4] = {WKing, 0, -1};
}

bool incheck(BBT &Board, int mvnm) {
    movegen(Board, mvnm+1);
    pos kg;
    for(int j = 0 ; j < 8 ; j++) {
        for(int k = 0 ; k < 8 ; k++) {
            if(Board[j][k].type%6 == 5 and (Board[j][k].type < 6) == (mvnm&1)) {
                kg = {j, k};
            }
        }
    }

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
            int tp = cur.type % 6;
            // if(cur.type > 5) continue;
            // 0: white, 1: black
            // bool color = cur.type < 6;
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
                if(Board[curr.F][curr.S].lstm == -1) {
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
                    if(good(Board, curr, kcur) and Board[kcur.F-clr][kcur.S].type%6 == 0 and Board[kcur.F-clr][kcur.S].lstm == movn-1 and !samecolor(Board, curr, {kcur.F-clr, kcur.S})) EPMv.push_back({curr, kcur});
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
                    // tcur.F += gg.F;
                    // tcur.S += gg.S;
                    add(tcur, gg);
                    while(inboard(tcur) and Board[tcur.F][tcur.S].type == VIDE) {
                        Moves.push_back({curr, tcur});
                        // tcur.F += gg.F;
                        // tcur.S += gg.S;
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
                // rook + bishop :pray:
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
                    // tcur.F += gg.F;
                    // tcur.S += gg.S;
                    add(tcur, gg);
                    while(inboard(tcur) and Board[tcur.F][tcur.S].type == VIDE) {
                        Moves.push_back({curr, tcur});
                        // tcur.F += gg.F;
                        // tcur.S += gg.S;
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

vector<BBT> boardgen(BBT &Board, int i) {
    // all *legal* boards from Board
    vector<BBT> res;
    movegen(Board, i);
    int kj = Moves.size();
    vector<move> fMoves = Moves;
    for(move k: EPMv) fMoves.push_back(k);
    // for(move k: fMoves) {
    //     cout << k.F.F << " " << k.F.S << " " << k.S.F << " " << k.S.S << endl;
    // }

    // if(i&1) cout << "WHITE TO PLAY:\n";
    // else cout << "BLACK TO PLAY:\n";
    // string a, b; cin >> a >> b;
    
    // Short castle
    movegen(Board, i+1);
    bool u = 1;
    int rank = 7*(!(i&1));
    for(move k: Moves) {
        if(k.S.F == rank and 4 <= k.S.S and k.S.S <= 6) u = 0;
    }

    // empty
    bool w = 1;
    for(int j = 5 ; j <= 6 ; j++) {
        if(Board[rank][j].type != VIDE) w = 0;
    }

    // cout << "H " << u << " " << w << endl;
    // no moves
    w = w and max(Board[rank][4].numm, Board[rank][7].numm) == 0;
    if(w and u) {
        BBT nBoard = Board;
        // copy(Board.begin(), Board.end(), nBoard);
        nBoard[rank][5] = nBoard[rank][7];
        nBoard[rank][6] = nBoard[rank][4];
        nBoard[rank][5].lstm = nBoard[rank][6].lstm = i;
        nBoard[rank][5].numm++;
        nBoard[rank][6].numm++;
        nBoard[rank][4] = nBoard[rank][7] = {VIDE, 0, -1};
        res.push_back(nBoard);
        // displ();
    }

    // Long Castle
    u = 1;
    rank = 7*(!(i&1));
    for(move k: Moves) {
        if(k.S.F == rank and 2 <= k.S.S and k.S.S <= 4) u = 0;
    }

    // empty
    w = 1;
    for(int j = 1 ; j <= 3 ; j++) {
        if(Board[rank][j].type != VIDE) w = 0;
    }

    // cout << "H " << u << " " << w << endl;
    // no moves
    w = w and max(Board[rank][0].numm, Board[rank][4].numm) == 0;
    if(w and u) {
        BBT nBoard = Board;
        // copy(Board, Board + 64, nBoard);
        // copy(Board.begin(), Board.end(), nBoard);
        nBoard[rank][2] = nBoard[rank][4];
        nBoard[rank][3] = nBoard[rank][0];
        nBoard[rank][2].lstm = nBoard[rank][3].lstm = i;
        nBoard[rank][2].numm++;
        nBoard[rank][3].numm++;
        nBoard[rank][0] = nBoard[rank][4] = {VIDE, 0, -1};
        res.push_back(nBoard);
        // displ();
    }
    

    // move g = {{7 - (a[1] - '1'), a[0] - 'a'}, {7-(b[1] - '1'), b[0] - 'a'}};
    // bool fnd = 0;
    for(int j = 0 ; j < fMoves.size() ; j++) {
        move pl = fMoves[j];    
        // BBT olbrd = Board;
        // copy(Board, Board + 64, olbrd);
        // copy(Board.begin(), Board.end(), olbrd);

        int clr = 1;
        if(!(i&1)) clr *= -1;
        Piece a = Board[pl.S.F][pl.S.S], b = Board[pl.F.F][pl.F.S];
        Board[pl.S.F][pl.S.S] = Board[pl.F.F][pl.F.S];
        Board[pl.F.F][pl.F.S] = {VIDE, 0, -1};
        Board[pl.S.F][pl.S.S].lstm = i;
        Board[pl.S.F][pl.S.S].numm++;

        bool ep = 0;
        Piece c;
        if(j >= kj) {
            ep = 1;
            c = Board[pl.S.F-clr][pl.S.S];
            Board[pl.S.F-clr][pl.S.S] = {VIDE, 0, -1}; // you ate in en passant :P
        }

        // king in check after the move?
        if(!incheck(Board, i)) res.push_back(Board);

        // pawn promo
        int q = 7*(!(i&1));
        if(Board[pl.S.F][pl.S.S].type%6 == 0 and pl.S.F == q) {
            for(int j = 1 ; j <= 4 ; j++) {
                Board[pl.S.F][pl.S.S].type = (Type)(j + (!(i&1))*6);
                res.push_back(Board);
            }
        }

        // copy(olbrd, olbrd + 64, Board);
        // copy(olbrd.begin(), olbrd.end(), Board);
        res.push_back(Board);

        Board[pl.F.F][pl.F.S] = b;
        Board[pl.S.F][pl.S.S] = a;
        if(ep) Board[pl.S.F-clr][pl.S.S] = c;
    }

    return res;
}

// this is stupid
// vector<BBT> boardgen(BBT CBoard, int i) {
//     Board = CBoard;
//     return back_boardgen(i);
// }