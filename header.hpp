#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <unordered_set>
using namespace std;
#define F first
#define S second
#define move pair<pair<int,int>, pair<int,int>>
// int move_num = 0;
using pos = pair<int, int>;
enum Type {
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
    int numm; // number of times moves
    int lstm; // last time moved
};

using BBT = array<array<Piece, 8>, 8>;

bool samecolor(BBT &CBoard, pos x, pos y);
bool inboard(pos x);
bool good(BBT &CBoard,pos x, pos y);
bool take(BBT &CBoard,pos x, pos y);
bool tng(BBT &CBoard,pos x, pos y);
void add(pos &x, pos y);
void displ(BBT CBoard);
void movegen(BBT &CBoard, int movn);
void build_board(BBT &CBoard);

bool incheck(BBT &CBoard, int mvnm);
vector<BBT> boardgen(BBT &CBoard, int i);
pair<int, int> minimax(BBT CBoard, int depth, int movn, int alpha, int beta);