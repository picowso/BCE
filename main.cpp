/*
* 
*
*/

#include "header.hpp"
BBT Board;
vector<move> Moves;
vector<move> EPMv;

int main() {
	build_board();
	cout << minimax(Board, 0, 0, -1000, 1000).F << endl;
}