// nnue.cpp: A basic NNUE for BCE
// https://www.chessprogramming.org/NNUE
#include "../header.hpp"
#define INPUT_SIZE 768
#define HL_SIZE 256
extern BBT Board;

struct NNUE {
	int hidden_w[INPUT_SIZE][HL_SIZE];
	int hidden_b[INPUT_SIZE];
	int output_w[2*HL_SIZE];
	int output_b;
};

NNUE NET;
const int Q = 1 << 16;
int SRelu(int x) {
	return (x < 0) ? 0 : (x > Q) ? Q : x;
}

int forward() {
	// input the board.
	int HD[HL_SIZE];
	memset(HD, 0, sizeof HD);
	for(int i = 0 ; i < HL_SIZE ; i++) {
		for(int j = 0 ; j < INPUT_SIZE ; j++) {
			HD[i] += SRelu(INP[i] * hidden_w[i][j] + hidden_b[j]);
		}
	}

	int ev = 0;
	for(int i = 0 ; i < HL_SIZE ; i++) {
		ev += SRelu(HD[]);
	}
}