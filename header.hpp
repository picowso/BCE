// header.hpp
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <stack>
#include <tuple>
#include <algorithm>
#include <unordered_map>
#include<ext/pb_ds/assoc_container.hpp>
#include<ext/pb_ds/tree_policy.hpp>
#include <random>
#include <chrono>
#include <cstring>
#define SETZ(p, i) p = p & ~(1 << i)
using namespace __gnu_pbds;
using namespace std;
const int DEPTH_LIMIT = 7;
const short INF = 30'000;
using u8 = unsigned char; // BYTE
using u64 = unsigned long long;
enum Piece : u8 {
    WP = 0,
    WN = 1,
    WB = 2,
    WR = 3,
    WQ = 4,
    WK = 5,
    BP = 6,
    BN = 7,
    BB = 8,
    BR = 9,
    BQ = 10,
    BK = 11,
    EMP = 12
};

using BBT = Piece[128];
struct CMove {
    u8 from; // from
    u8 to; // to
    Piece capture; // the piece captured
    Piece promo; // piece we're promoting to (EMP else pawn promotion)
    u8 flag; // 1 = en passant, 2 = long castle, 3 = short castle
};

array<short, 2> minimax(int depth, bool turn, int a = -INF, int b = INF);
void movegen(bool turn);
void domove(CMove Move, bool roll);
void build_board();
void undomove();
void printb();
bool incheck(bool kc, bool u = 0);
int perft(int i, bool turn);
string conv(CMove cmove);
bool color(int i);