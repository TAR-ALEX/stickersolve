#pragma once

#ifndef PRUNING_TREE_H
#define PRUNING_TREE_H

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <mutex>
#include <estd/ptr.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <estd/ostream_proxy.hpp>
#include <stickersolve/solver/puzzle.h>
#include <stickersolve/solver/puzzleState.h>
#include <stickersolve/pruning/redundancy.hpp>
#include <estd/thread_pool.hpp>
#include <stickersolve/config.hpp>

class Puzzle;

using namespace std;

struct PruningStates {
private:
    estd::clone_ptr<estd::thread_pool> threadManager = estd::thread_pool();
	// using a mutex to access the table ended up having poor performance, the chances of race conditions are close to zero
	// mutex tableMutex;
	boost::iostreams::mapped_file file;
	void performSizeCheck();
	void generate();
	void generateLevel(int lvl);
    void generateLevelSingleThread(int targetDepth, int initialDepth, vector<int> moves, vector<State> ss, vector<State> validMoves);
    void generateLevelMultiThread(int targetDepth, int detachDepth, vector<int> moves, vector<State> ss, vector<State> validMoves);
	uint64_t discardBits = 64;
	void initHashMask();
	uint8_t* data = nullptr;
	virtual bool canDiscardMoves(int movesAvailable, const vector<int>& moves);
	virtual State preHashTransformation(State s);
	vector<uint64_t> stats = vector<uint64_t>( 256,0 );
public:
	uint64_t getChecksum();
	double estimateSizeInGb();

	RedundancyTable redundancyTableInverse;
	Puzzle puzzle;
	estd::ostream_proxy log;
	bool useMmap = false;
	int hashSize = 16;
	int depth = 0;
	double maxSizeInGb = 0;
	string path = "";
	SolverConfig* cfg = &SolverConfig::global;

	void load(); // consumes a lot of memory
	void unload(); // frees the memory.
	
	bool cannotBeSolvedInLimit( int movesAvailable, const State& hash );
	bool cannotUseTable(int movesAvailable){
		return depth < movesAvailable;
	}
	
	void insert( State& hash, int moves );
	
	string getStats();
};

#endif // PRUNING_TREE_H