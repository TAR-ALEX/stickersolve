#include <map>
#include <algorithm>    // std::reverse
#include <stack>

#include "puzzle.h"

#include <unistd.h>

#include <boost/filesystem.hpp>

#include <chrono>
#include "pruningTree.h"


using namespace std::chrono;

void PruningStates::generate() {
	int& targetDepth = this->depth;

// 	pruningTable.hashSize = pruningHashSize;
// 	pruningTable.useMmap = useMmapForPruningTable;
// 	pruningTable.depth = targetDepth;
// 	pruningTable.maxSizeInGigasbytes = maxPruningTableSizeInGb;
// 	pruningTable.puzzle = this;
	
	auto start = high_resolution_clock::now();
	// Build the table incrementally, not currently used, keeping this here
	// for when automatic detection of a full table is implemented
	for ( int i = targetDepth; i <= targetDepth; i++ ) {
		log << "generating depth: " << i << endl;
		generateLevel( i );
	}
	
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<chrono::duration<double>>( stop - start );
	log << "\Pruning table (" << path << ") was successfuly generated in " << duration.count() << " seconds\n";
	log << "----------------------------------------------------------------\n";
}

void PruningStates::generateLevel (int lvl)
{
	auto& targetDepth = lvl;
    if ( targetDepth <= 0 ) return;
    //pruningStates.pruningStates.clear();
	vector<int> moves;
	moves.reserve( targetDepth );
	vector<State> ss;
	auto solvedState = puzzle.solvedState;
	ss.push_back( solvedState );

	insert( ss.back(), 0 );

	auto validMoves = puzzle.validMoves;
    int numChoices = validMoves.size();

    moves.clear();

	for ( ;; ) {
		if ( moves.size() < targetDepth) {
			moves.push_back( 0 );
			ss.emplace_back();
		} else {
			moves.back()++;
		}
		
		while ( moves.back() >= numChoices ) { retard:
			ss.pop_back();
			moves.pop_back();
			if ( moves.size() == 0 ) {
				goto done;
			}
			moves.back()++;
		}
		
		int movesLeft = targetDepth - moves.size();
		for ( ;; ) {
			if ( moves.back() >= numChoices ) goto retard;

			if ( canDiscardMoves( movesLeft, moves ) ) {
				moves.back()++;
				continue;
			}
			
			ss.back() = ss.rbegin()[1] + puzzle.validMoves[moves.back()];
			break;
		}
		
		if ( ss.back() == puzzle.solvedState ) {
			goto retard;
		}
		auto s = preHashTransformation( ss.back());
		insert (s, moves.size() );
	}
done:
	return;
}

bool PruningStates::canDiscardMoves( int movesAvailable, const vector<int>& moves ) {
	return redundancyTableInverse.contains(moves);
}


