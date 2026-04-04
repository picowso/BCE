// nnue.cpp: the nnue :-)
#include "header.hpp"

int hidden1_w[INPUT_SIZE][HL1_SIZE];
int hidden1_b[HL1_SIZE];
int output_w[HL1_SIZE];
int output_b;
int acc[HL1_SIZE];
const int QA = 255;
const int QB = 64;
const int SCALE = 400;

int gclamp(int n) {
	if(n < 0) return 0;
	if(n > QA) return QA;
	return n;
}

int act(int n) {
	n = gclamp(n);
	return n*n;
}

// read info from briwats.nnue
void build_nnue() {
	ifstream fl("briwats.nnue");
	int n, m; fl >> n >> m;
	if(n != INPUT_SIZE or m != HL1_SIZE) {
		cout << "nnue data is corrupt vro" << endl;
		exit(0);
	}

	for(int i = 0 ; i < n ; i++) {
		for(int j = 0 ; j < m ; j++) {
			double w;
			fl >> w;
			hidden1_w[i][j] = round(w * QA);
			// cout << i << " " << j << " " << hidden1_w[i][j] << endl;
		}
	}

	int n2; fl >> n2;
	if(n2 != HL1_SIZE) {
		cout << "nnue data is corrupt vro" << endl;
		exit(0);
	}

	for(int i = 0 ; i < n2 ; i++) {
		double w;
		fl >> w;
		hidden1_b[i] = round(w * QA);
	}

	int n3; fl >> n3;
	if(n3 != HL1_SIZE) {
		cout << "nnue data is corrupt vro" << endl;
		exit(0);
	}

	for(int i = 0 ; i < n3 ; i++) {
		double w;
		fl >> w;
		output_w[i] = round(w * QB);
	}

	double w;
	fl >> w;
	output_b = round(w * SCALE);
	// acc will be init'ede in build_board.
}

void acc_add(int x) {
	for(int i = 0 ; i < HL1_SIZE ; i++) acc[i] += hidden1_w[x][i];
}

void acc_sub(int x) {
	for(int i = 0 ; i < HL1_SIZE ; i++) acc[i] -= hidden1_w[x][i];
}

int evaluation() {
	long long res = 0;
	for(int i = 0 ; i < HL1_SIZE ; i++) {
		int a = act(acc[i] + hidden1_b[i]);
		res += a * output_w[i];
	}

	res /= QA;
	res += output_b;
	res *= SCALE;
	res /= QA*QB;
	return res;
}