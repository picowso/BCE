#include <iostream>

/* 
*	A first try at implementing MLPs for my hobby chess engine
*	BCE, there must be done some changes in order to use this
	for BCE (slight changes to be fair! NNUEs and MLPs!)
*/

const int INPUT_SIZE = 2;
const int HL_SIZE = 2;
double hidden_w[INPUT_SIZE][HL_SIZE];
double hidden_b[INPUT_SIZE];
double output_w;
double output_b;
double act(double x) {
	double u = min(max(x, 0.), 1.);
	return u*u;
}

double forward(double x) {
	
}

int main() {
	memset(hidden_w, 0, sizeof hidden_w);
	memset(hidden_b, 0, sizeof hidden_b);
}