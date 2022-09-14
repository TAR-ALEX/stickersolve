#pragma once

#ifndef SOLVER_3x3_CLL
#define SOLVER_3x3_CLL

#include <stickersolve/FingertricksSolver3x3.h>

using namespace std;

class Solver3x3Cll: public FingertricksSolver3x3{
public:
	using FingertricksSolver3x3::FingertricksSolver3x3;
// 	Solver3x3Cll(): FingertricksSolver3x3("U U2 U' R R2 R' F F2 F' r r' f f' u u' D D' D2 B B2 B'" ){
Solver3x3Cll(): FingertricksSolver3x3("U U2 U' R R2 R' F F2 F' r r' D D' D2 B B2 B'" ){
		puzzle.solvedState = {
			"U", "?", "U",
			"?", "U", "?",
			"U", "?", "U",

			"F", "?", "F",
			"F", "F", "F",
			"F", "F", "F",

			"R", "?", "R",
			"R", "R", "R",
			"R", "R", "R",

			"B", "?", "B",
			"B", "B", "B",
			"B", "B", "B",

			"L", "?", "L",
			"L", "L", "L",
			"L", "L", "L",

			"D", "D", "D",
			"D", "D", "D",
			"D", "D", "D"
		};
		redundancyTable.path = <stickersolve/redundancyCLL.table>;
		pruningTable.path = <stickersolve/pruningCLL.table>;
		pruningTable.depth = 7;
		pruningTable.puzzle = puzzle;
	}
};

#endif
