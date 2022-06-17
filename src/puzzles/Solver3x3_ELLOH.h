#pragma once

#ifndef SOLVER_3x3_RUr
#define SOLVER_3x3_RUr

#include "Solver3x3.h"

using namespace std;

class Solver3x3EllOH: public Solver3x3{
public:
	int maxFlickDiff = 0;
	Solver3x3EllOH(): Solver3x3("U U' R R' r r'" ){
		redundancyTable.path = "../redundancyELLOH.table";
		pruningTable.path = "../pruningELLOH.table";
		pruningTable.depth = 11;
		pruningTable.puzzle = puzzle;
	}
	
};

#endif
