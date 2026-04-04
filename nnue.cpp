#include "../header.hpp"
#define INPUT_SIZE 768
#define HL1_SIZE 128
#define pb push_back

int hidden1_w[INPUT_SIZE][HL1_SIZE];
int hidden1_b[HL1_SIZE];
int output_w[HL1_SIZE];
int output_b;
int acc[HL1_SIZE];
const double Q = 1024.;

// read info from briwats.nnue
void init_nnue() {
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
			hidden1_w[i][j] = round(w * Q);
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
		hidden1_b[i] = round(w * Q);
	}

	int n3; fl >> n3;
	if(n3 != HL1_SIZE) {
		cout << "nnue data is corrupt vro" << endl;
		exit(0);
	}

	for(int i = 0 ; i < n3 ; i++) {
		double w;
		fl >> w;
		output_w[i] = round(w * Q);
	}

	double w;
	fl >> w;
	output_b = round(w * Q);

	// TODO: set acc
}

void update_nnue(int old, int new) {
	for(int i = 0 ; i < HL1_SIZE ; i++) acc[i] -= hidden1_w[old][i];
	for(int i = 0 ; i < HL1_SIZE ; i++) acc[i] += hidden1_w[new][i];
}

int evaluation() {
	int res = 0;
	return res;
}