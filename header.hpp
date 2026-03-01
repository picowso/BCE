#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <stack>
#include <tuple>
#include <algorithm>
#include <unordered_map>
using namespace std;
const int DEPTH_LIMIT = 1;
const int INF = 1000000;
using u8 = int; // BYTE
enum Piece : u8 {
    P = 0,
    N = 1,
    B = 2,
    R = 3,
    Q = 4,
    K = 5,
    EMP = 6
};

using BBT = Piece[128];
struct CMove {
    u8 from; // from
    u8 to; // to
    Piece capture; // the piece captured
    Piece promo; // piece we're promoting to (EMP else pawn promotion)
    u8 flag; // 1 = en passant, 2 = long castle, 3 = short castle
};

array<int, 2> minimax(int depth, bool turn);
void movegen();
void domove(CMove Move, bool roll);
void build_board();
void undomove();