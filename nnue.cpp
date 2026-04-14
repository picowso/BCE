// nnue.cpp: the nnue :-)
#include "header.hpp"

extern BBT Board;
float hidden1_w[INPUT_SIZE][HL1_SIZE];
float hidden1_b[HL1_SIZE];
float output_w[HL1_SIZE];
float output_b;
float acc_w[HL1_SIZE]; // white view
float acc_b[HL1_SIZE]; // black view

// const int QA = 255;
// const int QB = 64;
// const int SCALE = 400;
Piece inverse(Piece n) {
    if(n == EMP) return EMP;
    if((int)n > 5) return (Piece)((int)n - 6);
    return (Piece)((int)n + 6);
}

int calc_nnue_index(int pos, bool color_view) {
    Piece pc = Board[pos];
    int sq = 8*(pos >> 4) + (pos&7);
    
    // // 1=white;
    // if(color_view == 0) {
    //     // pc ^= 1;
    //     pc = inverse(pc);
    //     sq ^= 56; // flipping for 0x88
    //     // nvm bug bruh, this isn't 0x88
    // }
    
    bool side = pc>5;
    return side*64*6 + 64 * (pc - 6 * side) + sq;
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
        inputw[calc_nnue_index(i, 1)] = 1;
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

float act(float n) {
	// if(n <= 0.f) return 0.;
	// if(n >= 1.f) return 1.;
	// return n*n;
	return max(0.f, n);
}

// read info from briwats.nnue
void build_nnue(string str) {
	ifstream fl(str);
	int n, m; fl >> n >> m;
	if(n != INPUT_SIZE or m != HL1_SIZE) {
		cout << "nnue data is corrupt vro" << endl;
		exit(0);
	}

	for(int i = 0 ; i < n ; i++) {
		for(int j = 0 ; j < m ; j++) {
			float w;
			fl >> w;
			hidden1_w[i][j] = w;
			// w = round(w * QA);
			// cout << i << " " << j << " " << hidden1_w[i][j] << endl;
		}
	}

	int n2; fl >> n2;
	if(n2 != HL1_SIZE) {
		cout << "nnue data is corrupt vro" << endl;
		exit(0);
	}

	for(int i = 0 ; i < n2 ; i++) {
		float w;
		fl >> w;
		hidden1_b[i] = w;
		// w = round(w * QA);
	}

	int n3; fl >> n3;
	if(n3 != HL1_SIZE) {
		cout << "nnue data is corrupt vro" << endl;
		exit(0);
	}

	for(int i = 0 ; i < n3 ; i++) {
		float w;
		fl >> w;
		output_w[i] = w;
		// round(w * QB)
	}

	float w;
	fl >> w;
	output_b = w;
}

// inverse for black
// x is given in 0x88
void acc_add(int x) {
	for(int i = 0 ; i < HL1_SIZE ; i++) {
		acc_w[i] += hidden1_w[calc_nnue_index(x, 1)][i];
		acc_b[i] += hidden1_w[calc_nnue_index(x, 0)][i];
	}
}

void acc_sub(int x) {
	for(int i = 0 ; i < HL1_SIZE ; i++) {
		acc_w[i] -= hidden1_w[calc_nnue_index(x, 1)][i];
		acc_b[i] -= hidden1_w[calc_nnue_index(x, 0)][i];
	}
}

int evaluation(bool pers) {
	// reset_acc();
	float res = output_b;
	for(int i = 0 ; i < HL1_SIZE ; i++) {
		float a;
		if(pers) a = act(acc_w[i] + hidden1_b[i]);
		else a = act(acc_b[i] + hidden1_b[i]);
		res += a * output_w[i];
	}

	if(!pers) return -(int)(100'000'000*res);
	return (int)(100'000'000*res);
}