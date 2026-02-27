#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <stack>
#include <tuple>
#include <algorithm>
using namespace std;
#define F first
#define S second
#define EMP {VIDE, 0}
#define move pair<pair<int,int>, pair<int,int>>
using uchar = unsigned char;

#define INF 10000000
using pos = pair<int, int>;
enum Type : uchar {
    WPawn = 0,
    WKnight = 1,
    WBishop = 2,
    WRook = 3,
    WQueen = 4,
    WKing = 5,
    BPawn = 6,
    BKnight = 7,
    BBishop = 8,
    BRook = 9,
    BQueen = 10,
    BKing = 11,
    VIDE = 12
};

struct Piece {
    Type type;
    // int numm; // number of times moves
    uchar lstm = 0; // last time moved
};

using fmov = tuple<uchar, uchar, Piece>;

using BBT = array<array<Piece, 8>, 8>;

// helper
bool samecolor(BBT &CBoard, pos x, pos y);
bool inboard(pos x);
bool good(BBT &CBoard,pos x, pos y);
bool take(BBT &CBoard,pos x, pos y);
bool tng(BBT &CBoard,pos x, pos y);
void add(pos &x, pos y);
void displ(BBT CBoard);
bool incheck(BBT &CBoard, int mvnm);

// move generation
void movegen(BBT &CBoard, int movn);
void build_board(BBT &CBoard);
vector<vector<fmov>> bmovesgen(BBT &Board, int i);
vector<BBT> boardgen(BBT &Board, int i);
void domove(BBT &Board, bool onstack, vector<fmov> &mov);
void undomove(BBT &Board);
vector<string> ucimovesgen(BBT &Board, int i);

// engine
pair<int, int> minimax(BBT &CBoard, int depth, int movn, int alpha, int beta);