#pragma once

#ifndef SOLVER_3x3_CLL_QTM
#define SOLVER_3x3_CLL_QTM

#include <stickersolve/Solver3x3_CLL.h>

using namespace std;

class Solver3x3CllQtm: public Solver3x3Cll{
public:
	Solver3x3CllQtm(): Solver3x3Cll("U U' R R' F F' r r' D D' B B'" ){
		redundancyTable.path = <stickersolve/redundancyCLL_q.table>;
		pruningTable.path = <stickersolve/pruningCLL_q.table>;
		pruningTable.depth = 8;
	}
};

#endif
