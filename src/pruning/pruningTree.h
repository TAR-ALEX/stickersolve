#pragma once

#ifndef PRUNING_TREE_H
#define PRUNING_TREE_H

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <boost/iostreams/device/mapped_file.hpp>

#include "Logging.tpp"
#include "puzzle.h"
#include "puzzleState.h"
#include "redundancy.hpp"


//#include "puzzleStateStack.h"

class Puzzle;

using namespace std;

struct PruningStates {
private:
	boost::iostreams::mapped_file file;
	void performSizeCheck();
	void generate();
	void generateLevel(int lvl);
	uint64_t discardBits = 64;
	void initHashMask();
	uint8_t* data = nullptr;
	virtual bool canDiscardMoves(int movesAvailable, const vector<int>& moves);
	virtual State preHashTransformation(State s);
public:
	uint64_t getChecksum();
	double estimateSizeInGb();

	RedundancyTable redundancyTableInverse;
	Puzzle puzzle;
	Logging log;
	bool useMmap = false;
	int hashSize = 16;
	int depth = 0;
	double maxSizeInGb = 0;
	string path = "";

	void load(); // consumes a lot of memory
	void unload(); // frees the memory.
	
	bool cannotBeSolvedInLimit( int movesAvailable, const State& hash );
	void insert( State& hash, int moves );
	
	string getStats();
};

#endif // PRUNING_TREE_H
