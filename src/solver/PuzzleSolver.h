#pragma once

#ifndef PUZZLE_SOLVER_H
#define PUZZLE_SOLVER_H

#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <fstream>

#include "../util/ResultQueue.h"
#include "../util/Logging.tpp"
#include "../util/threadmanager.hpp"
#include "../solver/puzzle.h"
#include "../pruning/redundancy.hpp"
#include "../pruning/pruningTree.h"
#include "../solver/puzzleState.h"
#include "../config.hpp"

using namespace std;

class Solver {
private:
	int numStickers = 1;

	void calculateStickerWidth();

    virtual void localInit();
	virtual void localInitReverse();

	void rawSolveMulti(
        vector<State> ss,
        int targetDepth,
        int detachDepth,
        vector<int> moves,
        vector<vector<int>>& gSolutions,
        mutex& gLock, volatile bool& stop,
        unsigned int numberOfSolutionsToGet = -1
    );

    void rawSolveRegular( 
        vector<State> ss, 
        int depth, 
        int startDepth, 
        vector<int> moves, 
        vector<vector<int>>& gSolutions, 
        mutex& gLock, 
        volatile bool& stop, 
        unsigned int numberOfSolutionsToGet = -1
    );

	vector<vector<int>> rawSolve( 
        State initial, 
        int depth, 
        bool inverse = false, 
        unsigned int numberOfSolutionsToGet = -1 
    );

	void rawSolveMultiInverse(
        vector<State> ss,
        int targetDepth,
        int detachDepth,
        vector<int> moves,
        vector<vector<int>>& gSolutions,
        mutex& gLock, volatile bool& stop,
        unsigned int numberOfSolutionsToGet = -1
    );

public: // TODO: make protected
	virtual bool canDiscardPosition(int movesAvailable, const State& state);
	virtual bool canDiscardMoves(int movesAvailable, const vector<int>& moves);
public:
    SolverConfig* cfg = &SolverConfig::global;

	Logging log;
	Puzzle puzzle;
    ThreadManager threadManager = ThreadManager();

    ResultQueue<vector<string>> asyncSolveVectors( Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1);
	ResultQueue<string> asyncSolveStrings( Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1);

	vector<vector<string>> solveVectors( Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1);
	vector<string> solveStrings( Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1);
	string solve( Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1);
	
	virtual void init(){};
	virtual void initReverse(){};
};

void printMoves( vector<string>& moveNames, vector<int> moves );

#endif // PUZZLE_SOLVER_H
