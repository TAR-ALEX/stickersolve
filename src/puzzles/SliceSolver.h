#pragma once

#ifndef SOLVER_3x3_SLICE_FINGERTRICKS
#define SOLVER_3x3_SLICE_FINGERTRICKS

#include "Solver3x3.h"

using namespace std;

class SliceSolver3x3: public Solver3x3{
public:

	SliceSolver3x3(string m): Solver3x3( m ) {

	};
	SliceSolver3x3(): Solver3x3( "U U' R R' F F' D D' r r' S S' E E' M M'" ) {
        pruningTable.depth = 6;
        redundancyTable.depth = 5;
        pruningTable.redundancyTableInverse.depth = 5;
	};
	map<int, int> gripSideEffects;
	set<int> alternatingSet;
	vector<set<int>> validMovesFromGrip;
	set<int> canOnlyStartWith;
	set<pair<int,int>> forbiddenPairs;
    int maxAlternatingDeviations = 1;
	int defaultGrip = 0;

	void init() {
		Solver3x3::init();

		gripSideEffects[puzzle.getMoveID( "R" )] = 1;
		gripSideEffects[puzzle.getMoveID( "r" )] = 1;

		gripSideEffects[puzzle.getMoveID( "R'" )] = 3; //-1
		gripSideEffects[puzzle.getMoveID( "r'" )] = 3; //-1

		gripSideEffects[puzzle.getMoveID( "R2" )] = 2;
		gripSideEffects[puzzle.getMoveID( "r2" )] = 2;

		alternatingSet = {
			puzzle.getMoveID( "R" ),
			puzzle.getMoveID( "r" ),
			puzzle.getMoveID( "R'" ),
			puzzle.getMoveID( "r'" ),
			puzzle.getMoveID( "R2" ),
			puzzle.getMoveID( "r2" ),
			puzzle.getMoveID( "M" ),
			puzzle.getMoveID( "M2" ),
			puzzle.getMoveID( "M'" ),
		};

		validMovesFromGrip = {
			{
				puzzle.getMoveID( "D'" ),
				puzzle.getMoveID( "D" ),
				puzzle.getMoveID( "D2" ),

				puzzle.getMoveID( "U'" ),
				puzzle.getMoveID( "U" ),
				puzzle.getMoveID( "U2" ),

// 				puzzle.getMoveID( "u'" ),
// 				puzzle.getMoveID( "u" ),
// 				puzzle.getMoveID( "u2" ),

				puzzle.getMoveID( "R'" ),
				puzzle.getMoveID( "R" ),
				puzzle.getMoveID( "R2" ),

				puzzle.getMoveID( "r'" ),
				puzzle.getMoveID( "r" ),
				puzzle.getMoveID( "r2" ),

// 				puzzle.getMoveID( "F" ),
// 				puzzle.getMoveID( "F'" ),

				puzzle.getMoveID( "S" ),
				puzzle.getMoveID( "S'" ),

// 				puzzle.getMoveID( "f" ),
// 				puzzle.getMoveID( "f'" ),

				puzzle.getMoveID( "M" ),
				puzzle.getMoveID( "M2" ),
				puzzle.getMoveID( "M'" ),
			},
			{
				puzzle.getMoveID( "D" ),
				puzzle.getMoveID( "D'" ),
				puzzle.getMoveID( "D2" ),

				puzzle.getMoveID( "U'" ),
				puzzle.getMoveID( "U" ),
				puzzle.getMoveID( "U2" ),

// 				puzzle.getMoveID( "u'" ),
// 				puzzle.getMoveID( "u" ),
// 				puzzle.getMoveID( "u2" ),

				puzzle.getMoveID( "B" ),
				puzzle.getMoveID( "B'" ),
				puzzle.getMoveID( "B2" ),

				puzzle.getMoveID( "F" ),
				puzzle.getMoveID( "F'" ),
				puzzle.getMoveID( "F2" ),

				puzzle.getMoveID( "R'" ),
				puzzle.getMoveID( "R" ),
				puzzle.getMoveID( "R2" ),

				puzzle.getMoveID( "r'" ),
				puzzle.getMoveID( "r" ),
				puzzle.getMoveID( "r2" ),

				puzzle.getMoveID( "M" ),
				puzzle.getMoveID( "M2" ),
				puzzle.getMoveID( "M'" ),

                puzzle.getMoveID( "E'" ),
				puzzle.getMoveID( "E" ),
				puzzle.getMoveID( "E2" ),

				puzzle.getMoveID( "S'" ),
				puzzle.getMoveID( "S" ),
				puzzle.getMoveID( "S2" ),
			},
			{
				// get out of this grip with an R based move, no continuation
				puzzle.getMoveID( "R'" ),
				puzzle.getMoveID( "R" ),
				puzzle.getMoveID( "R2" ),

				puzzle.getMoveID( "r'" ),
				puzzle.getMoveID( "r" ),
				puzzle.getMoveID( "r2" ),

				puzzle.getMoveID( "M" ),
				puzzle.getMoveID( "M2" ),
				puzzle.getMoveID( "M'" ),
			},
			{
				puzzle.getMoveID( "D" ),
				puzzle.getMoveID( "D'" ),
				puzzle.getMoveID( "D2" ),

				puzzle.getMoveID( "U'" ),
				puzzle.getMoveID( "U" ),
				puzzle.getMoveID( "U2" ),

// 				puzzle.getMoveID( "u'" ),
// 				puzzle.getMoveID( "u2" ),

				puzzle.getMoveID( "F" ),
				puzzle.getMoveID( "F2" ),
				puzzle.getMoveID( "F'" ),

				puzzle.getMoveID( "B" ),
				puzzle.getMoveID( "B'" ),
				puzzle.getMoveID( "B2" ),

// 				puzzle.getMoveID( "f" ),
// 				puzzle.getMoveID( "f2" ),
// 				puzzle.getMoveID( "f'" ),

				puzzle.getMoveID( "R'" ),
				puzzle.getMoveID( "R" ),
				puzzle.getMoveID( "R2" ),

				puzzle.getMoveID( "r'" ),
				puzzle.getMoveID( "r" ),
				puzzle.getMoveID( "r2" ),

				puzzle.getMoveID( "M" ),
				puzzle.getMoveID( "M2" ),
				puzzle.getMoveID( "M'" ),

				puzzle.getMoveID( "E'" ),
				puzzle.getMoveID( "E" ),
				puzzle.getMoveID( "E2" ),

				puzzle.getMoveID( "S'" ),
				puzzle.getMoveID( "S" ),
				puzzle.getMoveID( "S2" ),
			}
		};
		canOnlyStartWith = {
				puzzle.getMoveID( "R'" ),
				puzzle.getMoveID( "R" ),
				puzzle.getMoveID( "R2" ),

				puzzle.getMoveID( "r'" ),
				puzzle.getMoveID( "r" ),
				puzzle.getMoveID( "r2" ),

				puzzle.getMoveID( "U'" ),
				puzzle.getMoveID( "U" ),
				puzzle.getMoveID( "U2" ),

// 				puzzle.getMoveID( "F" ),
// 				puzzle.getMoveID( "F'" ),

// 				puzzle.getMoveID( "f" ),
// 				puzzle.getMoveID( "f'" ),

                puzzle.getMoveID( "E'" ),
				puzzle.getMoveID( "E" ),
				puzzle.getMoveID( "E2" ),

				puzzle.getMoveID( "S'" ),
				puzzle.getMoveID( "S" ),
				puzzle.getMoveID( "S2" ),

				puzzle.getMoveID( "M" ),
				puzzle.getMoveID( "M2" ),
				puzzle.getMoveID( "M'" ),
		};
		forbiddenPairs = {};
		{
			vector<int> rWideMoves = {
				puzzle.getMoveID( "r" ), puzzle.getMoveID( "r2" ), puzzle.getMoveID( "r'" )
			};
			vector<int> rMoves = {
				puzzle.getMoveID( "R" ), puzzle.getMoveID( "R2" ), puzzle.getMoveID( "R'" ),
			};
            vector<int> mMoves = {
				puzzle.getMoveID( "M" ), puzzle.getMoveID( "M2" ), puzzle.getMoveID( "M'" )
			};
            vector<int> sMoves = {
				puzzle.getMoveID( "S" ), puzzle.getMoveID( "S2" ), puzzle.getMoveID( "S'" )
			};
            vector<int> eMoves = {
				puzzle.getMoveID( "E" ), puzzle.getMoveID( "E2" ), puzzle.getMoveID( "E'" )
			};
			vector<int> fMoves = {
				puzzle.getMoveID( "F" ), puzzle.getMoveID( "F2" ), puzzle.getMoveID( "F'" ),
			};
			for(int i = 0; i < 3; i++){
				for(int j = 0; j < 3; j++){
					forbiddenPairs.insert({rWideMoves[i], rMoves[j]});
					forbiddenPairs.insert({rMoves[i], rWideMoves[j]});

                    forbiddenPairs.insert({rWideMoves[i], mMoves[j]});
					forbiddenPairs.insert({mMoves[i], rWideMoves[j]});

                    forbiddenPairs.insert({rMoves[i], mMoves[j]});
					forbiddenPairs.insert({mMoves[i], rMoves[j]});

                    forbiddenPairs.insert({eMoves[i], mMoves[j]});
					forbiddenPairs.insert({mMoves[i], eMoves[j]});

                    forbiddenPairs.insert({sMoves[i], mMoves[j]});
					forbiddenPairs.insert({mMoves[i], sMoves[j]});

                    forbiddenPairs.insert({sMoves[i], eMoves[j]});
					forbiddenPairs.insert({eMoves[i], sMoves[j]});
				}
			}
		}
	}
	bool canDiscardMoves( int movesAvailable, const vector<int>& moves ) {
		if ( redundancyTable.contains( moves ) ) return true;

		int grip = defaultGrip;
		int lastMove = 0;
		int allowedMMoves = 4;
		bool first = true;
		bool lastMMoveWasM = false;
        int alternatingDeviations = 0;

		for ( auto& move : moves ) {
// 			if(move == MMove || move == M2Move || move == M3Move) {
// 				allowedMMoves--;
// 				if(allowedMMoves < 0) return true;
// 			}
			if ( first ) {
				if ( !canOnlyStartWith.count( move ) ) return true;
			} else {
				if(forbiddenPairs.count({lastMove, move})) return true;
			}

			if ( !validMovesFromGrip.at( grip ).count( move ) ) return true;

			if ( gripSideEffects.count( move ) )
				grip = ( grip + gripSideEffects.at( move ) ) & 0X3;

            if(!first){
                if(alternatingSet.count(lastMove) == alternatingSet.count(move)) {
                    alternatingDeviations++;
                    if(alternatingDeviations > maxAlternatingDeviations) return true;
                };
            }
            lastMove = move;
            first = false;
		}

		return false;
	}
	bool canDiscardPosition( int movesAvailable, const State& state ){
		return Solver3x3::canDiscardPosition(movesAvailable, state);
	}
};

#endif

