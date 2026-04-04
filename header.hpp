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
#include <bitset>
#include <cstring>
#include <fstream>
#define pb push_back
#define SETZ(p, i) p = p & ~i;
#define TT_SIZE 33554432
#define TT_HASH 33554431
#define INPUT_SIZE 768
#define HL1_SIZE 128
using namespace __gnu_pbds;
using namespace std;
const int ITER_LIMIT = 3'000'000;
const int INF = 100'000'000;
const int DEPTH_MAX = 12;
const int DEPTH_MIN = 4;
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
    u8 flag; // 1 = en passant, 2 = s castle, 3 = l castle
};

int minimax(int dn, int depth, bool turn, int a = -INF, int b = INF, int ply = 0);
int mtdf(bool turn, int f, int depth, int ply);
int perft(int i, bool turn);

void build_board();
void build_attack(bool u);
void build_fen(string str);
void build_zob();
void build_nnue();

void movegen(bool turn);
void domove(CMove Move);
void undomove();
void printb();
bool incheck(bool kc);
string conv(CMove cmove);
bool color(int i);
bool bincheck(int i);
u64 upd(bool turn);

// NNUE:
void acc_sub();
void acc_add();
void build_nnue();