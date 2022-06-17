#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <chrono>
#include <algorithm>
#include <tuple>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <cmath>
#include <sstream>
#include "puzzle.h"


using namespace std::chrono;

using namespace std;


bool Puzzle::checkIfAllMovesHaveInverses() {
	for ( auto move : validMoves ) {
		bool hasInverse = false;

		for ( auto potentialInverse : validMoves ) {
			if ( ( move + potentialInverse ).isNOP() ) {
				hasInverse = true;
				break;
			} 
		}

		if ( !hasInverse ) {
			return false;
		}
	}

	return true;
}


// string Puzzle::toString() {
//   string result = "[";
//   for (int i = 0; i < solvedState.size(); i++) {
//     if (i != 0) {
//       result += ", ";
//     }
//     result += to_string(solvedState[i]);
//   }
//   return result + "]";
// }

Puzzle::Puzzle() {}
Puzzle::Puzzle( State s ) {
	solvedState = s;
}

void Puzzle::addMove( string name, State move ) {
	validMoves.push_back( move );
	moveNames.push_back( name );
}

int Puzzle::getMoveID( string name ) {
	for ( int i = 0; i < moveNames.size(); i++ ) {
		if ( moveNames[i] == name ) return i;
	}

	return -1;
}

State& Puzzle::getMove( int id ) {
	return validMoves[id];
}

State& Puzzle::getMove( string name ) {
	return validMoves[getMoveID( name )];
}

vector<uint8_t> Puzzle::compressState( State& s ) {
	vector<uint8_t> result;
	for(int i = 0; i < s.size(); i++){
		result.push_back((uint8_t)(s[i]));
	}
	return result;	
}

map<int, int> Puzzle::buildInverseTable() {
	map<int, int> result;
	for (int i = 0; i < validMoves.size(); i++) {
		for ( int j = 0; j < validMoves.size(); j++ ) {
			if ( ( validMoves[i] + validMoves[j] ).isNOP() ) {
				result.insert({i,j});
				break;
			}
		}
	}
	return result;
}

uint64_t Puzzle::getChecksum() const {
	const uint64_t goldenRatioConstant = 0x9e3779b97f4a7c15;
	uint64_t result = 0;
	
	result ^= solvedState.toHash();
	
	for ( int i = 0; i < validMoves.size(); i++ ) {
		result ^= validMoves[i].toHash() * ( ( goldenRatioConstant << i ) | ( goldenRatioConstant >> ( 64 - i ) ) );
	}

	return result;
}



