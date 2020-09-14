#pragma once

#ifndef SOLVER_3x3_RUFrBD
#define SOLVER_3x3_RUFrBD

#include "../PuzzleSolver.h"
#include <regex>

using namespace std;

class Solver3x3: public Solver{
public:
	RedundancyTable redundancyTable;
	PruningStates pruningTable;
	Solver3x3(): Solver3x3("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'"){
		
	};
	Solver3x3(string allowedMoves): Solver(){
		std::regex regex{R"([\s]+)"}; // split on space
		std::sregex_token_iterator it{allowedMoves.begin(), allowedMoves.end(), regex, -1};
		std::set<std::string> movesToInit{it, {}};
		
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
		
		{
			State move = {
				6, 3, 0,      7, 4, 1,      8, 5, 2,
				18, 19, 20,   12, 13, 14,   15, 16, 17,
				27, 28, 29,   21, 22, 23,   24, 25, 26,
				36, 37, 38,   30, 31, 32,   33, 34, 35,
				9, 10, 11,    39, 40, 41,   42, 43, 44,
				45, 46, 47,   48, 49, 50,   51, 52, 53
			};

			if ( movesToInit.count( "U" ) ) {
				puzzle.addMove( "U", move );
			}

			if ( movesToInit.count( "U2" ) ) {
				puzzle.addMove( "U2", move * 2 );
			}

			if ( movesToInit.count( "U'" ) ) {
				puzzle.addMove( "U'", move * 3 );
			}
		}
		
		{
			State move = {
				0, 1, 11,     3, 4, 14,     6, 7, 17,
				9, 10, 47,    12, 13, 50,   15, 16, 53,
				24, 21, 18,   25, 22, 19,   26, 23, 20,
				8, 28, 29,    5, 31, 32,    2, 34, 35,
				36, 37, 38,   39, 40, 41,   42, 43, 44,
				45, 46, 33,   48, 49, 30,   51, 52, 27
			};

			if ( movesToInit.count( "R" ) ) {
				puzzle.addMove( "R", move );
			}

			if ( movesToInit.count( "R2" ) ) {
				puzzle.addMove( "R2", move * 2 );
			}

			if ( movesToInit.count( "R'" ) ) {
				puzzle.addMove( "R'", move * 3 );
			}
		}
		
		{
			State move = {
				0, 1, 2,      3, 4, 5,      44, 41, 38,
				15, 12, 9,    16, 13, 10,   17, 14, 11,
				6, 19, 20,    7, 22, 23,    8, 25, 26,
				27, 28, 29,   30, 31, 32,   33, 34, 35,
				36, 37, 45,   39, 40, 46,   42, 43, 47,
				24, 21, 18,   48, 49, 50,   51, 52, 53
			};

			if ( movesToInit.count( "F" ) ) {
				puzzle.addMove( "F", move );
			}

			if ( movesToInit.count( "F2" ) ) {
				puzzle.addMove( "F2", move * 2 );
			}

			if ( movesToInit.count( "F'" ) ) {
				puzzle.addMove( "F'", move * 3 );
			}
		}
		
		{
			State move = {
				35, 1, 2, 32, 4, 5, 29, 7, 8,
				0, 10, 11, 3, 13, 14, 6, 16, 17,
				18, 19, 20, 21, 22, 23, 24, 25, 26,
				27, 28, 51, 30, 31, 48, 33, 34, 45,

				42, 39, 36, 43, 40, 37, 44, 41, 38,
				9, 46, 47, 12, 49, 50, 15, 52, 53
			};

			if ( movesToInit.count( "L" ) ) {
				puzzle.addMove( "L", move );
			}

			if ( movesToInit.count( "L2" ) ) {
				puzzle.addMove( "L2", move * 2 );
			}

			if ( movesToInit.count( "L'" ) ) {
				puzzle.addMove( "L'", move * 3 );
			}
		}
		
		{
			State move = {
				0, 10, 11, 3, 13, 14, 6, 16, 17,
				9, 46, 47, 12, 49, 50, 15, 52, 53,
				24, 21, 18, 25, 22, 19, 26, 23, 20,
				8, 7, 29, 5, 4, 32, 2, 1, 35,
				36, 37, 38, 39, 40, 41, 42, 43, 44,
				45, 34, 33, 48, 31, 30, 51, 28, 27
			};

			if ( movesToInit.count( "r" ) ) {
				puzzle.addMove( "r", move );
			}

			if ( movesToInit.count( "r2" ) ) {
				puzzle.addMove( "r2", move * 2 );
			}

			if ( movesToInit.count( "r'" ) ) {
				puzzle.addMove( "r'", move * 3 );
			}
		}
		
		{
			State move = {
				0, 1, 2, 3, 4, 5, 6, 7, 8,

				9, 10, 11, 12, 13, 14,   42, 43, 44, // <>,
				18, 19, 20, 21, 22, 23,  15, 16, 17, // <>
				27, 28, 29, 30, 31, 32,  24, 25, 26, // <>
				36, 37, 38, 39, 40, 41,  33, 34, 35, // <>

				51, 48, 45, 52, 49, 46, 53, 50, 47
			};

			if ( movesToInit.count( "D" ) ) {
				puzzle.addMove( "D", move );
			}

			if ( movesToInit.count( "D2" ) ) {
				puzzle.addMove( "D2", move * 2 );
			}

			if ( movesToInit.count( "D'" ) ) {
				puzzle.addMove( "D'", move * 3 );
			}
		}
		
		{
			State move = {
				20, 23, 26, 3, 4, 5, 6, 7, 8,
				9, 10, 11, 12, 13, 14, 15, 16, 17,
				18, 19, 53, 21, 22, 52, 24, 25, 51,
				33, 30, 27, 34, 31, 28, 35, 32, 29,
				2, 37, 38, 1, 40, 41, 0, 43, 44,
				45, 46, 47, 48, 49, 50, 36, 39, 42
			};

			if ( movesToInit.count( "B" ) ) {
				puzzle.addMove( "B", move );
			}

			if ( movesToInit.count( "B2" ) ) {
				puzzle.addMove( "B2", move * 2 );
			}

			if ( movesToInit.count( "B'" ) ) {
				puzzle.addMove( "B'", move * 3 );
			}
		}
		
		{
			State move = {
				0, 10, 2, 3, 13, 5, 6, 16, 8,
				9, 46, 11, 12, 49, 14, 15, 52, 17,
				18, 19, 20, 21, 22, 23, 24, 25, 26,
				27, 7, 29, 30, 4, 32, 33, 1, 35,
				36, 37, 38, 39, 40, 41, 42, 43, 44,
				45, 34, 47, 48, 31, 50, 51, 28, 53
			};

			if ( movesToInit.count( "M'" ) ) {
				puzzle.addMove( "M'", move );
			}

			if ( movesToInit.count( "M2" ) ) {
				puzzle.addMove( "M2", move * 2 );
			}

			if ( movesToInit.count( "M" ) ) {
				puzzle.addMove( "M", move * 3 );
			}
		}
		
		{
			State move = {
				0, 1, 2,      43, 40, 37,      44, 41, 38,
				15, 12, 9,    16, 13, 10,   17, 14, 11,
				6, 3, 20,    7, 4, 23,    8, 5, 26,
				27, 28, 29,   30, 31, 32,   33, 34, 35,
				36, 48, 45,   39, 49, 46,   42, 50, 47,
				24, 21, 18,   25, 22, 19,   51, 52, 53
			};

			if ( movesToInit.count( "f" ) ) {
				puzzle.addMove( "f", move );
			}

			if ( movesToInit.count( "f2" ) ) {
				puzzle.addMove( "f2", move * 2 );
			}

			if ( movesToInit.count( "f'" ) ) {
				puzzle.addMove( "f'", move * 3 );
			}
		}
		
		{
			State move = {
				6, 3, 0,      7, 4, 1,      8, 5, 2,
				18, 19, 20,   21, 22, 23,   15, 16, 17,
				27, 28, 29,   30, 31, 32,   24, 25, 26,
				36, 37, 38,   39, 40, 41,   33, 34, 35,
				9, 10, 11,    12, 13, 14,   42, 43, 44,
				45, 46, 47,   48, 49, 50,   51, 52, 53
			};

			if ( movesToInit.count( "u" ) ) {
				puzzle.addMove( "u", move );
			}

			if ( movesToInit.count( "u2" ) ) {
				puzzle.addMove( "u2", move * 2 );
			}

			if ( movesToInit.count( "u'" ) ) {
				puzzle.addMove( "u'", move * 3 );
			}
		}

		log.forward(cerr);
		redundancyTable.depth = 3; // 6
		redundancyTable.maxSizeInGigabytes = 1.24;
		redundancyTable.path = "../redundancy.table";
		redundancyTable.puzzle = puzzle;
		redundancyTable.log.forward(cerr);

		pruningTable.redundancyTableInverse.depth = 6;
		pruningTable.redundancyTableInverse.maxSizeInGigabytes = 1.24;
		//pruningTable.redundancyTableInverse.path = "../redundancyInverse.table";
		pruningTable.redundancyTableInverse.inverse = true;
		pruningTable.redundancyTableInverse.puzzle = puzzle;
		pruningTable.redundancyTableInverse.log.forward(cerr);
		
		pruningTable.maxSizeInGb = 9.0;
		pruningTable.depth = 7;// 8 HTM 10 rfu
		pruningTable.hashSize = 32;
		pruningTable.path = "../pruning.table";
		pruningTable.useMmap = false;
		pruningTable.puzzle = puzzle;
		pruningTable.log.forward(cerr);
		cerr << "constructor done\n";
	}
	void init() {
		redundancyTable.load();
 		pruningTable.load();
	}
	bool canDiscardMoves(int movesAvailable, const vector<int>& moves){
		return redundancyTable.contains(moves);
	}
	bool canDiscardPosition( int movesAvailable, const State& state ){
		return pruningTable.cannotBeSolvedInLimit(movesAvailable, state);
	}
};

#endif
