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
	displ(minimax(Board, 0, 0).F);
}