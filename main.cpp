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
	vector<BBT> u = boardgen(0);
	cout << u.size() << endl;
}