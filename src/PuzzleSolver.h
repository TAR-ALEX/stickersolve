#pragma once

#ifndef PUZZLE_SOLVER_H
#define PUZZLE_SOLVER_H

#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <fstream>

#include "Logging.tpp"
#include "puzzle.h"
#include "redundancy.hpp"
#include "pruningTree.h"
#include "puzzleState.h"

using namespace std;

class Solver {
private:
	int numStickers = 1;
	void calculateStickerWidth();
	
	vector<vector<int>> rawSolve( State initial, int depth, bool inverse = false, unsigned int numberOfSolutionsToGet = -1 );
	vector<vector<int>> rawSolveRegular( State initial, int depth, unsigned int numberOfSolutionsToGet = -1 );
	vector<vector<int>> rawSolveInverse( State initial, int depth, unsigned int numberOfSolutionsToGet = -1 );
public:
	virtual bool canDiscardPosition(int movesAvailable, const State& state);
	virtual bool canDiscardMoves(int movesAvailable, const vector<int>& moves);
	
	Logging log;
	Puzzle puzzle;

	vector<vector<string>> solveVectors( State initial, int depth );
	vector<string> solveStrings( State initial, int depth );
	string solve( State initial, int depth );
	
	virtual void init();
	virtual void initReverse();
};

void printMoves( vector<string>& moveNames, vector<int> moves );

#endif // PUZZLE_SOLVER_H
