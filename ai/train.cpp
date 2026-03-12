// train.cpp: simple MLP implementation, trains on a set of labeled FENs
// Quantizate weights and biases to plug in the NNUE
// I'll have 2 hidden layers: https://www.youtube.com/watch?v=kqvccCpYX3I | https://kaifishr.github.io/2021/01/14/micro-mlp.htm
#include <iostream>
#include <random>
#include <cstring>
using namespace std;
using dbl = double;
#define INPUT_SIZE 1
#define HL1_SIZE 256
#define HL2_SIZE 64

dbl hidden1_w[INPUT_SIZE][HL1_SIZE];
dbl hidden2_w[HL1_SIZE][HL2_SIZE];
dbl hidden1_b[HL1_SIZE];
dbl hidden2_b[HL2_SIZE];
dbl output_w[HL2_SIZE];
dbl output_b;
int input[INPUT_SIZE];
dbl sigmoid(dbl x) {
	return 1 / (1 + exp(-x));
}

dbl d_sigmoid(dbl x) {
	return sigmoid(x) * (1 - sigmoid(x));
}

dbl feed_forward() {
    dbl z1[HL1_SIZE], a1[HL1_SIZE];
    for(int i = 0; i < HL1_SIZE; i++) {
        z1[i] = hidden1_b[i];
        for(int j = 0; j < INPUT_SIZE; j++) z1[i] += input[j] * hidden1_w[j][i];
        a1[i] = sigmoid(z1[i]);
    }

    dbl z2[HL2_SIZE], a2[HL2_SIZE];
    for(int i = 0; i < HL2_SIZE; i++) {
        z2[i] = hidden2_b[i];
        for(int j = 0; j < HL1_SIZE; j++) z2[i] += a1[j] * hidden2_w[j][i];
        a2[i] = sigmoid(z2[i]);
    }

    dbl zout = output_b;
    for(int i = 0; i < HL2_SIZE; i++) zout += a2[i] * output_w[i];
    return sigmoid(zout);
}

dbl loss(dbl i, dbl ex) {
	input[0] = i;
	dbl u = feed_forward();
	return 0.5 * (u - ex) * (u - ex);
}

dbl alpha = 1;
// we learning cuh we so intelligent
void backprop(dbl i, dbl ex) {
    // redoing feed forward
    input[0] = i;
    dbl z1[HL1_SIZE], a1[HL1_SIZE];
    for(int i = 0; i < HL1_SIZE; i++) {
        z1[i] = hidden1_b[i];
        for(int j = 0; j < INPUT_SIZE; j++) z1[i] += input[j] * hidden1_w[j][i];
        a1[i] = sigmoid(z1[i]);
    }

    dbl z2[HL2_SIZE], a2[HL2_SIZE];
    for(int i = 0; i < HL2_SIZE; i++) {
        z2[i] = hidden2_b[i];
        for(int j = 0; j < HL1_SIZE; j++) z2[i] += a1[j] * hidden2_w[j][i];
        a2[i] = sigmoid(z2[i]);
    }

    dbl zout = output_b;
    for(int i = 0; i < HL2_SIZE; i++) zout += a2[i] * output_w[i];
    dbl aout = sigmoid(zout);
    dbl delta = (aout - ex) * d_sigmoid(zout);

    // output
    for(int i = 0; i < HL2_SIZE; i++) output_w[i] -= alpha * delta * a2[i];
    output_b -= alpha * delta;

    // hidden 2
    dbl delta2[HL2_SIZE];
    for(int i = 0; i < HL2_SIZE; i++) {
        delta2[i] = output_w[i] * delta * d_sigmoid(z2[i]);
    }

    for(int i = 0; i < HL2_SIZE; i++) {
        for(int j = 0; j < HL1_SIZE; j++) {
            // dL/d(hidden2_w[j][i]) = delta2[i] * a1[j]
            hidden2_w[j][i] -= alpha * delta2[i] * a1[j];
        }
        hidden2_b[i] -= alpha * delta2[i];
    }

    // hidden 1
    dbl delta1[HL1_SIZE];
    for(int i = 0; i < HL1_SIZE; i++) {
        dbl sum = 0;
        for(int j = 0; j < HL2_SIZE; j++) {
            sum += hidden2_w[i][j] * delta2[j];
        }
        delta1[i] = sum * d_sigmoid(z1[i]);
    }

    // Update hidden1 weights and biases
    for(int i = 0; i < HL1_SIZE; i++) {
        for(int j = 0; j < INPUT_SIZE; j++) {
            // dL/d(hidden1_w[j][i]) = delta1[i] * input[j]
            hidden1_w[j][i] -= alpha * delta1[i] * input[j];
        }

        hidden1_b[i] -= alpha * delta1[i];
    }
}

int main() {
	memset(hidden1_w, 0, sizeof hidden1_w);
	memset(hidden2_w, 0, sizeof hidden2_w);
	memset(hidden1_b, 0, sizeof hidden1_b);
	memset(hidden2_b, 0, sizeof hidden2_b);
	memset(output_w, 0, sizeof output_w);
	output_b = 0;
	// train it on i^2 :sob:
	for(dbl i = 0. ; i < 1 ; i += 0.01) {
		backprop(i, i*i);
		cout << "step: " << i*100 << " loss: " << loss(i, i*i) << endl;
	}
}