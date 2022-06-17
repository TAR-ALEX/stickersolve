#pragma once

#ifndef SOLVER_3x3_TLS
#define SOLVER_3x3_TLS

#include "FingertricksSolver3x3.h"

using namespace std;

class Solver3x3Tls: public Solver3x3{
public:
	Solver3x3Tls(): Solver3x3("U U2 U' R R2 R' F F2 F' D D' r r' M M'" ){
		puzzle.solvedState = {
			"U", "?", "U",
			"?", "U", "?",
			"U", "?", "U",

			"?", "?", "?",
			"F", "F", "F",
			"F", "F", "F",

			"?", "?", "?",
			"R", "R", "R",
			"R", "R", "R",

			"?", "?", "?",
			"B", "B", "B",
			"B", "B", "B",

			"?", "?", "?",
			"L", "L", "L",
			"L", "L", "L",

			"D", "D", "D",
			"D", "D", "D",
			"D", "D", "D"
		};
		redundancyTable.path = "../redundancyTLS.table";
		pruningTable.path = "../pruningTLS.table";
		pruningTable.depth = 7;
		pruningTable.hashSize = 29;
		pruningTable.puzzle = puzzle;
	}
};

#endif
