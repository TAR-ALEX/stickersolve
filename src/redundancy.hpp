#pragma once

#ifndef PUZZLE_REDUNDANCY_H
#define PUZZLE_REDUNDANCY_H

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <boost/iostreams/device/mapped_file.hpp>

#include "Logging.tpp"
#include "puzzle.h"
#include "puzzleState.h"

class Puzzle;

class RedundancyTable {
private:
	
	uint64_t lastMovesToHash( const vector<int>& e ) const;
	
	void performSizeCheck();
	void generate();
public:
	uint64_t getChecksum();
	//vector<uint64_t> stats;
	uint8_t* data = nullptr;
	string path = "";
	double maxSizeInGigabytes = 0;
	int depth = 0;
	bool inverse = false;
	Logging log;
	Puzzle puzzle;
	
	double estimateSizeInGb();

	bool contains( const vector<int>& e ) const;
	void insert( const vector<int>& e );
	
	void load(); // consumes a lot of memory
	void unload(); // frees the memory.
	
	string toString();
};

#endif // PUZZLE_REDUNDANCY_H
