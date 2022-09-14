#pragma once

#ifndef PUZZLE_SOLVER_H
#define PUZZLE_SOLVER_H

#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <memory>

#include <estd/thread_safe_queue.h>
#include <estd/ostream_proxy.hpp>
#include <estd/thread_pool.hpp>
#include <estd/ptr.hpp>
#include <stickersolve/solver/puzzle.h>
#include <stickersolve/pruning/redundancy.hpp>
#include <stickersolve/pruning/pruningTree.h>
#include <stickersolve/solver/puzzleState.h>
#include <stickersolve/config.hpp>

using namespace std;

class Solver {
private:
	int numStickers = 1;
    estd::clone_ptr<estd::thread_pool> threadManager = estd::thread_pool();

	void calculateStickerWidth();

    virtual void localInit();
	virtual void localInitReverse();

	void rawSolveMulti(
        vector<State> ss,
        int targetDepth,
        int detachDepth,
        vector<int> moves,
        shared_ptr<estd::thread_safe_queue<vector<int>>>& gSolutions,
        volatile bool& stop,
        unsigned int numberOfSolutionsToGet = -1
    );

    void rawSolveRegular( 
        vector<State> ss, 
        int depth, 
        int startDepth, 
        vector<int> moves, 
        shared_ptr<estd::thread_safe_queue<vector<int>>>& gSolutions, 
        volatile bool& stop, 
        unsigned int numberOfSolutionsToGet = -1
    );

	void rawSolve( 
        shared_ptr<estd::thread_safe_queue<vector<int>>>,
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
        shared_ptr<estd::thread_safe_queue<vector<int>>>& gSolutions,
        volatile bool& stop,
        unsigned int numberOfSolutionsToGet = -1
    );

protected:
	virtual bool canDiscardPosition(int movesAvailable, const State& state);
	virtual bool canDiscardMoves(int movesAvailable, const vector<int>& moves);
    Puzzle puzzle;
    virtual void initReverse(){}; // move to public once working
public:
    SolverConfig* cfg = &SolverConfig::global;

	estd::ostream_proxy log;

    shared_ptr<estd::thread_safe_queue<vector<string>>> asyncSolveVectors( Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1);
	shared_ptr<estd::thread_safe_queue<string>> asyncSolveStrings( Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1);

	vector<vector<string>> solveVectors( Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1);
	vector<string> solveStrings( Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1);
	string solve( Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1);
	
	virtual void init(){};
};

void printMoves( vector<string>& moveNames, vector<int> moves );

#endif // PUZZLE_SOLVER_H
