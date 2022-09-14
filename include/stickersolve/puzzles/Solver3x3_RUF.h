#pragma once

#ifndef SOLVER_3x3_RUF
#define SOLVER_3x3_RUF

#include <stickersolve/FingertricksSolver3x3.h>

using namespace std;

class Solver3x3RUF: public FingertricksSolver3x3{
public:
	using FingertricksSolver3x3::FingertricksSolver3x3;
Solver3x3RUF(): FingertricksSolver3x3("U U2 U' R R2 R' F F2 F'" ){
		redundancyTable.path = <stickersolve/redundancyRUF.table>;
		pruningTable.path = <stickersolve/pruningRUF.table>;
		pruningTable.depth = 9;
		pruningTable.puzzle = puzzle;
	}
};

#endif
