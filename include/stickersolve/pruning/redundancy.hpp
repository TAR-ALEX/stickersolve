#pragma once

#ifndef PUZZLE_REDUNDANCY_H
#define PUZZLE_REDUNDANCY_H

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <boost/iostreams/device/mapped_file.hpp>

#include <estd/ostream_proxy.hpp>
#include <stickersolve/solver/puzzle.h>
#include <stickersolve/solver/puzzleState.h>
#include <stickersolve/config.hpp>

class Puzzle;

class RedundancyTable {
private:
	uint64_t lastMovesToHash( const vector<int>& e ) const;
	
	void performSizeCheck();
	void generate();
public:
	uint64_t getChecksum();
	vector<uint64_t> stats;
	uint8_t* data = nullptr;
	string path = "";
	// double maxSizeInGigabytes = 0;
	int depth = 0;
	bool inverse = false;
	estd::ostream_proxy log;
	Puzzle puzzle;
	SolverConfig* cfg = &SolverConfig::global;
	
	double estimateSizeInGb();

	bool contains( const vector<int>& e ) const;
	void insert( const vector<int>& e );
	
	void load(); // consumes a lot of memory
	void unload(); // frees the memory.
	
	string getStats();
};

#endif // PUZZLE_REDUNDANCY_H
