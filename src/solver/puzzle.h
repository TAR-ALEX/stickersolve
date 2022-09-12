#pragma once

#ifndef PUZZLE_H
#define PUZZLE_H

#include <iostream>
#include <vector>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <map>
#include <fstream>

#include "../solver/puzzleState.h"

using namespace std;

class Puzzle {
	
private:
	vector<uint8_t> compressState( State& s ); // WARNING: This works only when number of stickers is less than 256, (dont use for redundancyTable in the future)

	State state;
	State solvedState;
	vector<State> validMoves;
	vector<string> moveNames;

public:
	vector<int> invertMoves(vector<int>);
	uint64_t getChecksum() const;
	bool checkIfAllMovesHaveInverses();
	map<int, int> buildInverseTable();
	
	Puzzle(const Puzzle&) = default;
	Puzzle(Puzzle&&) = default;
	Puzzle& operator= (const Puzzle& x) = default;
	Puzzle& operator= (Puzzle&& x) = default;
	Puzzle& operator= (initializer_list<int> il);
	int& operator[] (int i);

	void keepOnlyMoves(string);
	void deleteMoves(string);
	
	void keepOnlyMoves(set<string>);
	void deleteMoves(set<string>);
	
	Puzzle();
	Puzzle( State s );

	void addMove( string name, State move );
	void deleteMove(string name);
	State& getMove( string name );
	State& getMove( int name );
	int getMoveID( string name );

	
    void applyMoves(string moves);

	string toString();

	operator State();
	
	friend class Solver;
	friend class RedundancyTable;
	friend class PruningStates;
};

void printMoves( vector<string>& moveNames, vector<int> moves );

#endif // PUZZLE_H
