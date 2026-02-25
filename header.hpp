#include <iostream>
#include <vector>
#include <string>
#include <array>
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

bool samecolor(pos x, pos y);
bool inboard(pos x);
bool good(pos x, pos y);
bool take(pos x, pos y);
bool tng(pos x, pos y);
void add(pos &x, pos y);
void displ();
void movegen(int movn);
void build_board();
vector<BBT> boardgen(int i);