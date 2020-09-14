#pragma once

#ifndef SOLVER_3x3_RUFrBD_FINGERTRICKS
#define SOLVER_3x3_RUFrBD_FINGERTRICKS

#include "Solver3x3.h"

using namespace std;

class FingertricksSolver3x3: public Solver3x3{
public:
// 	FingertricksSolver3x3(): Solver3x3("U U2 U' R R2 R' F F2 F' D D2 D' r r2 r' B B2 B' M M2 M'"){
// 		
// 	};
// 	FingertricksSolver3x3(): Solver3x3( "U U' R R2 R' F F' D D' r r' B B' M M' f f' u u'" ) {
// 
// 	};
	FingertricksSolver3x3(string m): Solver3x3( m ) {
		
	};
	FingertricksSolver3x3(): Solver3x3( "U U' R R2 R' F F' r r' M M'" ) {
		pruningTable.depth = 8;
	};
	map<int, int> gripSideEffects;
	set<int> alternatingSet;
	vector<set<int>> validMovesFromGrip;
	set<int> canOnlyStartWith;
	set<pair<int,int>> forbiddenPairs;
	int MMove = 0;
	int M2Move = 0;
	int M3Move = 0;
	int defaultGrip = 0;
	
	void init() {
		Solver3x3::init();
		
		MMove = puzzle.getMoveID( "M" );
		M2Move = puzzle.getMoveID( "M2" );
		M3Move = puzzle.getMoveID( "M'" );
		
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
				
				puzzle.getMoveID( "u'" ),
				puzzle.getMoveID( "u" ),
				puzzle.getMoveID( "u2" ),

				puzzle.getMoveID( "R'" ),
				puzzle.getMoveID( "R" ),
				puzzle.getMoveID( "R2" ),

				puzzle.getMoveID( "r'" ),
				puzzle.getMoveID( "r" ),
				puzzle.getMoveID( "r2" ),

				puzzle.getMoveID( "F" ),
				puzzle.getMoveID( "F'" ),
				
				puzzle.getMoveID( "f" ),
				puzzle.getMoveID( "f'" ),
				
				puzzle.getMoveID( "M" ),
				puzzle.getMoveID( "M2" ),
				puzzle.getMoveID( "M'" ),
			},
			{
				puzzle.getMoveID( "D" ),
				puzzle.getMoveID( "D2" ),

				puzzle.getMoveID( "U'" ),
				puzzle.getMoveID( "U" ),
				puzzle.getMoveID( "U2" ),
				
				puzzle.getMoveID( "u'" ),
				puzzle.getMoveID( "u" ),
				puzzle.getMoveID( "u2" ),

				puzzle.getMoveID( "B" ),
				puzzle.getMoveID( "B2" ),

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
				puzzle.getMoveID( "D2" ),

				puzzle.getMoveID( "U'" ),
				puzzle.getMoveID( "U2" ),
				
				puzzle.getMoveID( "u'" ),
				puzzle.getMoveID( "u2" ),

				puzzle.getMoveID( "F" ),
				puzzle.getMoveID( "F2" ),
				puzzle.getMoveID( "F'" ),
				
				puzzle.getMoveID( "f" ),
				puzzle.getMoveID( "f2" ),
				puzzle.getMoveID( "f'" ),

				puzzle.getMoveID( "R'" ),
				puzzle.getMoveID( "R" ),
				puzzle.getMoveID( "R2" ),

				puzzle.getMoveID( "r'" ),
				puzzle.getMoveID( "r" ),
				puzzle.getMoveID( "r2" ),
				
				puzzle.getMoveID( "M" ),
				puzzle.getMoveID( "M2" ),
				puzzle.getMoveID( "M'" ),
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
				
				puzzle.getMoveID( "F" ),
				puzzle.getMoveID( "F'" ),
				
				puzzle.getMoveID( "f" ),
				puzzle.getMoveID( "f'" ),
				
				puzzle.getMoveID( "M" ),
				puzzle.getMoveID( "M2" ),
				puzzle.getMoveID( "M'" ),
		};
		forbiddenPairs = { // R r combos wont matter if allowing M moves.
// 			{puzzle.getMoveID( "R" ), puzzle.getMoveID( "r2" )},
// 			{puzzle.getMoveID( "R" ), puzzle.getMoveID( "r" )},
// 			{puzzle.getMoveID( "R'" ), puzzle.getMoveID( "r2" )},
// 			{puzzle.getMoveID( "R'" ), puzzle.getMoveID( "r'" )},
// 			{puzzle.getMoveID( "R2" ), puzzle.getMoveID( "r'" )},
// 			{puzzle.getMoveID( "R2" ), puzzle.getMoveID( "r" )},
// 			
// 			{puzzle.getMoveID( "r" ), puzzle.getMoveID( "R2" )},
// 			{puzzle.getMoveID( "r" ), puzzle.getMoveID( "R" )},
// 			{puzzle.getMoveID( "r'" ), puzzle.getMoveID( "R2" )},
// 			{puzzle.getMoveID( "r'" ), puzzle.getMoveID( "R'" )},
// 			{puzzle.getMoveID( "r2" ), puzzle.getMoveID( "R'" )},
// 			{puzzle.getMoveID( "r2" ), puzzle.getMoveID( "R" )},
		};
		{
			vector<int> pair1 = {
				puzzle.getMoveID( "r" ), puzzle.getMoveID( "r2" ), puzzle.getMoveID( "r'" )
			};
			vector<int> pair2 = {
				puzzle.getMoveID( "R" ), puzzle.getMoveID( "R2" ), puzzle.getMoveID( "R'" ),
			};
			for(auto m1: pair1){
				for(auto m2 : pair2){
					forbiddenPairs.insert({m1, m2});
					forbiddenPairs.insert({m2, m1});
				}
			}
		}
		{
			vector<int> pair1 = {
				puzzle.getMoveID( "M" ), puzzle.getMoveID( "M2" ), puzzle.getMoveID( "M'" )
			};
			vector<int> pair2 = {
				puzzle.getMoveID( "F" ), puzzle.getMoveID( "F2" ), puzzle.getMoveID( "F'" ),
				puzzle.getMoveID( "f" ), puzzle.getMoveID( "f2" ), puzzle.getMoveID( "f'" ),
				puzzle.getMoveID( "B" ), puzzle.getMoveID( "B2" ), puzzle.getMoveID( "B'" ),
				puzzle.getMoveID( "D" ), puzzle.getMoveID( "D2" ), puzzle.getMoveID( "D'" ),
			};
			for(auto m1: pair1){
				for(auto m2 : pair2){
					forbiddenPairs.insert({m1, m2});
					forbiddenPairs.insert({m2, m1});
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

		for ( auto& move : moves ) {
// 			if(move == MMove || move == M2Move || move == M3Move) {
// 				allowedMMoves--;
// 				if(allowedMMoves < 0) return true;
// 			}
			if ( first ) {
				if ( !canOnlyStartWith.count( move ) ) return true;

				first = false;
			} else {
				if(lastMMoveWasM && move == MMove) return true;
				if(forbiddenPairs.count({lastMove, move})) return true;
				if ( !alternatingSet.count( move ) && !alternatingSet.count( lastMove ) ) {
					return true;
				}
			}

			if ( move == MMove ) {
				lastMMoveWasM = true;
			} else if ( move == M2Move || move == M3Move ) {
				lastMMoveWasM = false;
			}

			lastMove = move;

			if ( !validMovesFromGrip.at( grip ).count( move ) ) return true;

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
