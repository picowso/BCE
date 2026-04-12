// nnue.cpp: the nnue :-)
#include "header.hpp"

float hidden1_w[INPUT_SIZE][HL1_SIZE];
float hidden1_b[HL1_SIZE];
float output_w[HL1_SIZE];
float output_b;
float acc_w[HL1_SIZE]; // white view
float acc_b[HL1_SIZE]; // black view

// const int QA = 255;
// const int QB = 64;
// const int SCALE = 400;

float act(float n) {
	if(n <= 0.f) return 0.;
	if(n >= 1.f) return 1.;
	return n*n;
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

void accw_add(int x) {
	for(int i = 0 ; i < HL1_SIZE ; i++) acc_w[i] += hidden1_w[x][i];
}

void accw_sub(int x) {
	for(int i = 0 ; i < HL1_SIZE ; i++) acc_w[i] -= hidden1_w[x][i];
}

// show add inverted
void accb_add(int x) {
	for(int i = 0 ; i < HL1_SIZE ; i++) acc_b[i] += hidden1_w[x][i];
}

void accb_sub(int x) {
	for(int i = 0 ; i < HL1_SIZE ; i++) acc_b[i] -= hidden1_w[x][i];
}

int evaluation() {
	float res = output_b;
	for(int i = 0 ; i < HL1_SIZE ; i++) {
		float a = act(acc_w[i] + hidden1_b[i]);
		res += a * output_w[i];
	}

	return (int)res;
}