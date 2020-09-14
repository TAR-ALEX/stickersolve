#include "puzzle.h"

#include <unistd.h>
#include <stack>
#include <algorithm>
#include <sstream>
#include <chrono>
#include "PuzzleSolver.h"

using namespace std::chrono;
using namespace std;


void Solver::init() {

}

void Solver::initReverse() {
	throw runtime_error(
	string() +
	"This has not been implemented, remove this exception to get this functionality with bugs related to restricted moves, will not work with custom move pruning"
	);
}



void printMoves(vector<string>& moveNames, vector<int> moves){
  for(auto move : moves){
    cout << moveNames[move] << " ";
  }
  cout << endl;
}

vector<vector<int>> Solver::rawSolve( State initial, int targetDepth, bool inverse, unsigned int numberOfSolutionsToGet ) {
	if ( inverse ) {
		initReverse();
	} else {
		init();
	}

	auto start = high_resolution_clock::now();

	vector<vector<int>> solutions;

	if ( inverse ) {
		solutions = rawSolveInverse( initial, targetDepth, numberOfSolutionsToGet );
	} else {
		solutions = rawSolveRegular( initial, targetDepth, numberOfSolutionsToGet );
	}

	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<chrono::duration<double>>( stop - start );

	log << "Solving took: " << duration.count() << " seconds.\n";
	log << "----------------------------------------------------\n";

	return solutions;
}


vector<vector<int>> Solver::rawSolveInverse( State initial, int targetDepth, unsigned int numberOfSolutionsToGet ) {
	if ( !puzzle.checkIfAllMovesHaveInverses() ) {
		throw runtime_error( 
			string() + 
			"The provided puzzle does not have inverses for every single move, " + 
			"this makes it a bad candidate for generator scrambles, a separate pruning table would "+
			"be required and might be supported in the future." 
		);
	}
	auto solutions = rawSolveRegular(initial, targetDepth, numberOfSolutionsToGet);
	auto inverse = puzzle.buildInverseTable();
	for(auto& solution : solutions){
		// Flip the order
		reverse(solution.begin(),solution.end());
		// Flip the move
		for(int i = 0; i < solution.size(); i++) solution[i] = inverse[solution[i]];
	}
	return solutions;
}


vector<vector<int>> Solver::rawSolveRegular( State initial, int targetDepth, unsigned int numberOfSolutionsToGet ) {
	if ( targetDepth <= 0 ) return {};

	State final = puzzle.solvedState;

	vector<vector<int>> solutions;

	vector<int> moves;

	moves.reserve( targetDepth );

	vector<State> ss;

	ss.push_back( initial );

	int numChoices = puzzle.validMoves.size();

	// basically a counting algorithm with cary rippled over.
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

			if ( canDiscardPosition( movesLeft, ss.back() ) ) {
				moves.back()++;
				continue;
			}

			break;
		}
		
		if ( ss.back() == final ) {
			if ( solutions.size() < numberOfSolutionsToGet ) {
				solutions.push_back( moves );
				goto retard;
			} else {
				goto done;
			}
		}
	}
done:
	return solutions;
}

vector<vector<string>> Solver::solveVectors(State initial, int targetDepth) {
  auto solutions = rawSolve(initial, targetDepth, false);
  sort(
    solutions.begin(),
    solutions.end(),
    [](const vector<int> & a, const vector<int> & b){
      return a.size() < b.size();
    }
  );
  vector<vector<string>> solutionStrings;
  for(auto solution: solutions){
    vector<string> solutionString;
    for(auto moveId: solution)
      solutionString.push_back(puzzle.moveNames[moveId]);
    solutionStrings.push_back(solutionString);
  }


  return solutionStrings;
}

vector<std::string> Solver::solveStrings( State initial, int depth ) {
	stringstream ss;
	vector<std::string> result;
	auto solutions =  solveVectors( initial, depth );

	for ( auto solution : solutions ) {
		bool first = true;

		for ( auto move : solution ) {
			if ( !first ) {
				ss << " ";
			} else {
				first = false;
			}

			ss << move;
		}

		result.push_back( ss.str() );
		ss.clear();
	}

	return result;
}

string Solver::solve( State initial, int depth ) {
	stringstream ss;
	auto solutions =  solveVectors( initial, depth );
	
	int lastSize = 0;
	for ( auto solution : solutions ) {
		if ( solution.size() > lastSize ) {
			lastSize = solution.size();
			ss << "\nSOLUTIONS OF LENGTH " << lastSize << ":\n";
		}

		bool first = true;

		for ( auto move : solution ) {
			if ( !first ) {
				ss << " ";
			} else {
				first = false;
			}

			ss << move;
		}

		ss << endl;
	}

	return ss.str();
}

bool Solver::canDiscardMoves( int movesAvailable, const vector<int>& moves ) {
	return false;
}

bool Solver::canDiscardPosition( int movesAvailable, const State& state ) {
	return false;
}



