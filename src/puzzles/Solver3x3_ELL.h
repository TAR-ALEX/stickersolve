#pragma once

#ifndef SOLVER_3x3_RUrM
#define SOLVER_3x3_RUrM

#include "../PuzzleSolver.h"

using namespace std;

class Solver3x3Ell: public Solver{
public:
	RedundancyTable redundancyTable;
	PruningStates pruningTable;
	
	map<int, int> gripSideEffects;
	
	Solver3x3Ell(): Solver(){
		puzzle = Puzzle( 
			{
				"U", "U", "U",
				"U", "U", "U",
				"U", "U", "U",

				"F", "F", "F",
				"F", "F", "F",
				"F", "F", "F",

				"R", "R", "R",
				"R", "R", "R",
				"R", "R", "R",

				"B", "B", "B",
				"B", "B", "B",
				"B", "B", "B",

				"L", "L", "L",
				"L", "L", "L",
				"L", "L", "L",

				"D", "D", "D",
				"D", "D", "D",
				"D", "D", "D"
			}
		);
		
		puzzle.addMove(
		"U", {
			6, 3, 0,      7, 4, 1,      8, 5, 2,
			18, 19, 20,   12, 13, 14,   15, 16, 17,
			27, 28, 29,   21, 22, 23,   24, 25, 26,
			36, 37, 38,   30, 31, 32,   33, 34, 35,
			9, 10, 11,    39, 40, 41,   42, 43, 44,
			45, 46, 47,   48, 49, 50,   51, 52, 53
		}
		);
		puzzle.addMove( "U2", puzzle.getMove( "U" ) * 2 );
		puzzle.addMove( "U'", puzzle.getMove( "U" ) * 3 );

		puzzle.addMove(
		"R", {
			0, 1, 11,     3, 4, 14,     6, 7, 17,
			9, 10, 47,    12, 13, 50,   15, 16, 53,
			24, 21, 18,   25, 22, 19,   26, 23, 20,
			8, 28, 29,    5, 31, 32,    2, 34, 35,
			36, 37, 38,   39, 40, 41,   42, 43, 44,
			45, 46, 33,   48, 49, 30,   51, 52, 27
		}
		);
		puzzle.addMove( "R2", puzzle.getMove( "R" ) * 2 );
		puzzle.addMove( "R'", puzzle.getMove( "R" ) * 3 );

// 		puzzle.addMove(
// 		"F", {
// 			0, 1, 2,      3, 4, 5,      44, 41, 38,
// 			15, 12, 9,    16, 13, 10,   17, 14, 11,
// 			6, 19, 20,    7, 22, 23,    8, 25, 26,
// 			27, 28, 29,   30, 31, 32,   33, 34, 35,
// 			36, 37, 45,   39, 40, 46,   42, 43, 47,
// 			24, 21, 18,   48, 49, 50,   51, 52, 53
// 		}
// 		);
// 		puzzle.addMove( "F2", puzzle.getMove( "F" ) * 2 );
// 		puzzle.addMove( "F'", puzzle.getMove( "F" ) * 3 );

		puzzle.addMove(
		"r", {
			0, 10, 11, 3, 13, 14, 6, 16, 17,
			9, 46, 47, 12, 49, 50, 15, 52, 53,
			24, 21, 18, 25, 22, 19, 26, 23, 20,
			8, 7, 29, 5, 4, 32, 2, 1, 35,
			36, 37, 38, 39, 40, 41, 42, 43, 44,
			45, 34, 33, 48, 31, 30, 51, 28, 27
		}
		);
		//puzzle.addMove( "r2", puzzle.getMove( "r" ) * 2 );
		puzzle.addMove( "r'", puzzle.getMove( "r" ) * 3 );

// 		puzzle.addMove(
// 		"D", {
// 			0, 1, 2, 3, 4, 5, 6, 7, 8,
// 
// 			9, 10, 11, 12, 13, 14,   42, 43, 44, // <>,
// 			18, 19, 20, 21, 22, 23,  15, 16, 17, // <>
// 			27, 28, 29, 30, 31, 32,  24, 25, 26, // <>
// 			36, 37, 38, 39, 40, 41,  33, 34, 35, // <>
// 
// 			51, 48, 45, 52, 49, 46, 53, 50, 47
// 		}
// 		);
// 		puzzle.addMove( "D2", puzzle.getMove( "D" ) * 2 );
// 		puzzle.addMove( "D'", puzzle.getMove( "D" ) * 3 );

// 		puzzle.addMove(
// 		"B", {
// 			20, 23, 26, 3, 4, 5, 6, 7, 8,
// 			9, 10, 11, 12, 13, 14, 15, 16, 17,
// 			18, 19, 53, 21, 22, 52, 24, 25, 51,
// 			33, 30, 27, 34, 31, 28, 35, 32, 29,
// 			2, 37, 38, 1, 40, 41, 0, 43, 44,
// 			45, 46, 47, 48, 49, 50, 36, 39, 42
// 		}
// 		);
// 		puzzle.addMove( "B2", puzzle.getMove( "B" ) * 2 );
// 		puzzle.addMove( "B'", puzzle.getMove( "B" ) * 3 );


		puzzle.addMove(
	        "M'",
	    {
	        0,10,2,3,13,5,6,16,8,
			9,46,11,12,49,14,15,52,17,
			18,19,20,21,22,23,24,25,26,
			27,7,29,30,4,32,33,1,35,
			36,37,38,39,40,41,42,43,44,
			45,34,47,48,31,50,51,28,53
	    }
	    );
	    puzzle.addMove("M2'", puzzle.getMove("M'")*2);

		log.forward(cerr);
		redundancyTable.depth = 3; // 6
		redundancyTable.maxSizeInGigabytes = 1.24;
		redundancyTable.path = "../redundancy.table";
		redundancyTable.puzzle = puzzle;
		redundancyTable.log.forward(cerr);

		pruningTable.redundancyTableInverse.depth = 6;
		pruningTable.redundancyTableInverse.maxSizeInGigabytes = 1.24;
		pruningTable.redundancyTableInverse.path = "../redundancyInverse.table";
		pruningTable.redundancyTableInverse.inverse = true;
		pruningTable.redundancyTableInverse.puzzle = puzzle;
		pruningTable.redundancyTableInverse.log.forward(cerr);
		
		pruningTable.maxSizeInGb = 9.0;
		pruningTable.depth = 10;// 8 HTM 10 rfu
		pruningTable.hashSize = 33;
		pruningTable.path = "../pruning.table";
		pruningTable.useMmap = false;
		pruningTable.puzzle = puzzle;
		pruningTable.log.forward(cerr);
		cerr << "constructor done\n";
	}
	set<int> rMoves;
	void init() {
		redundancyTable.load();
 		pruningTable.load();
		
		gripSideEffects[puzzle.getMoveID( "R" )] = 1;
		gripSideEffects[puzzle.getMoveID( "r" )] = 1;

		gripSideEffects[puzzle.getMoveID( "R'" )] = 3; //-1
		gripSideEffects[puzzle.getMoveID( "r'" )] = 3; //-1

		gripSideEffects[puzzle.getMoveID( "R2" )] = 2;
		gripSideEffects[puzzle.getMoveID( "r2" )] = 2;
		
		rMoves = {
			puzzle.getMoveID( "R2" ),
			puzzle.getMoveID( "r2" ),
			
			puzzle.getMoveID( "R" ),
			puzzle.getMoveID( "r" ),
			
			puzzle.getMoveID( "R'" ),
			puzzle.getMoveID( "r'" ),
		};
		
		
	}
	bool canDiscardMoves( int movesAvailable, const vector<int>& moves ) {
		if ( redundancyTable.contains( moves ) ) return true;

		int grip = 0;

		for ( auto& move : moves ) {
			if ( grip == 4 && !rMoves.count(move) ) return true;

			if ( gripSideEffects.count( move ) )
				grip = ( grip + gripSideEffects.at( move ) ) & 0X3;
		}

		return false;
	}
	bool canDiscardPosition( int movesAvailable, const State& state ){
		return pruningTable.cannotBeSolvedInLimit(movesAvailable, state);
	}
};

#endif
