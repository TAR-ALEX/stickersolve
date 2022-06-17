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
public:
	vector<int> invertMoves(vector<int>);
	uint64_t getChecksum() const;
	bool checkIfAllMovesHaveInverses();
	map<int, int> buildInverseTable();
	
	State state;
	State solvedState;
	vector<State> validMoves;
	vector<string> moveNames;

	void keepOnlyMoves(string);
	void deleteMoves(string);
	
	void keepOnlyMoves(set<string>);
	void deleteMoves(set<string>);

	string toString();
	Puzzle();
	Puzzle( State s );

	void addMove( string name, State move );
	void deleteMove(string name);
	State& getMove( string name );
	State& getMove( int name );
	int getMoveID( string name );
    void applyMoves(string moves);
	
	vector<uint8_t> compressState( State& s ); // WARNING: This works only when number of stickers is less than 256, (dont use for redundancyTable in the future)
};

void printMoves( vector<string>& moveNames, vector<int> moves );

#endif // PUZZLE_H
