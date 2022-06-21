#include "../solver/puzzle.h"

#include <unistd.h>
#include <stack>
#include <algorithm>
#include <sstream>
#include <chrono>
#include "PuzzleSolver.h"
#include "../util/threadmanager.hpp"

using namespace std::chrono;
using namespace std;


void Solver::localInit() {
	threadManager.targetThreads = cfg->targetThreads;
}

void Solver::localInitReverse() {
	threadManager.targetThreads = cfg->targetThreads;
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
		localInitReverse();
		initReverse();
	} else {
		localInit();
		init();
	}

	int numChoices = puzzle.validMoves.size();

	int detachDepth = 0;
    int detachWidth = 1;

    while(detachWidth < threadManager.targetThreads){
        detachWidth *= numChoices;
        detachDepth++;
        if(detachDepth >= targetDepth) {
            break;
        }
    }

    detachDepth++;
    detachWidth *= numChoices;


	log << "Starting solver with "<<threadManager.targetThreads<<" threads\n";
	log << "detach depth = "<<detachDepth<<"\n";
	log << "detach width = "<<detachWidth<<"\n";
	log << "----------------------------------------------------\n";

	auto start = high_resolution_clock::now();

	vector<vector<int>> solutions;
    vector<int> moves;
    vector<State> ss;
    ss.push_back(initial);
    mutex gMutex;
    volatile bool terminateEarly = false;

	if ( inverse ) {
        rawSolveMultiInverse(ss, targetDepth, detachDepth, moves, solutions, gMutex, terminateEarly, numberOfSolutionsToGet);
	} else {
        rawSolveMulti(ss, targetDepth, detachDepth, moves, solutions, gMutex, terminateEarly, numberOfSolutionsToGet);
        threadManager.waitThreads();
	}



	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<chrono::duration<double>>( stop - start );

	log << "Solving took: " << duration.count() << " seconds.\n";
	log << "----------------------------------------------------\n";

	return solutions;
}


void Solver::rawSolveMultiInverse(
        vector<State> ss,
        int targetDepth,
        int detachDepth,
        vector<int> moves,
        vector<vector<int>>& gSolutions,
        mutex& gLock, volatile bool& stop,
        unsigned int numberOfSolutionsToGet
){
	if ( !puzzle.checkIfAllMovesHaveInverses() ) {
		throw runtime_error( 
			string() + 
			"The provided puzzle does not have inverses for every single move, " + 
			"this makes it a bad candidate for generator scrambles, a separate pruning table would "+
			"be required and might be supported in the future." 
		);
	}
	rawSolveMulti(ss, targetDepth, detachDepth, moves, gSolutions, gLock, stop, numberOfSolutionsToGet);
	auto inverse = puzzle.buildInverseTable();
    threadManager.waitThreads();
	for(auto& solution : gSolutions){
		// Flip the order
		reverse(solution.begin(),solution.end());
		// Flip the move
		for(int i = 0; i < solution.size(); i++) solution[i] = inverse[solution[i]];
	}

}

void Solver::rawSolveMulti(
    vector<State> ss,
    int targetDepth,
    int detachDepth,
    vector<int> moves,
    vector<vector<int>>&
    gSolutions,
    mutex& gLock,
    volatile bool &stop,
    unsigned int numberOfSolutionsToGet
) {
    if ( targetDepth <= 0 ) return;

	State final = puzzle.solvedState;


	int numChoices = puzzle.validMoves.size();

    moves.push_back(0);
    ss.emplace_back();
    int movesLeft = targetDepth - moves.size();
    while ( moves.back() < numChoices ) {
        if(canDiscardMoves( movesLeft, moves )){
            moves.back()++; continue;
        }

        ss.back() = ss.rbegin()[1] + puzzle.validMoves[moves.back()];

        if(canDiscardPosition( movesLeft, ss.back() )){
            moves.back()++; continue;
        }

        if(stop) return;

        if ( ss.back() == final ) {
            gLock.lock();
			if ( gSolutions.size() < numberOfSolutionsToGet ) {
				gSolutions.push_back( moves );
                stop = true;
                gLock.unlock();
				continue;
			} else {
                gLock.unlock();
				return;
			}
		}

        if(moves.size() >= detachDepth){
            threadManager.requestThread();
            thread t(
                [&gLock, &gSolutions, &stop, ss, moves, targetDepth, numberOfSolutionsToGet, this] {
                    rawSolveRegular(ss, targetDepth, moves.size(), moves, gSolutions, gLock, stop, numberOfSolutionsToGet);
                    threadManager.deleteThread();
                }
            );
            t.detach();
        }else{
            rawSolveMulti( ss, targetDepth, detachDepth, moves, gSolutions, gLock, stop, numberOfSolutionsToGet);
        }
        moves.back()++;
    }
}

//( vector<State> ss, int targetDepth, int startDepth, vector<int> moves, unsigned int numberOfSolutionsToGet )

void Solver::rawSolveRegular(vector<State> ss, int targetDepth, int startDepth, vector<int> moves, vector<vector<int>>& solutions, mutex& gLock,volatile bool &stop, unsigned int numberOfSolutionsToGet) {
	if ( targetDepth <= 0 ) return;

	State final = puzzle.solvedState;

	moves.reserve( targetDepth );

	int numChoices = puzzle.validMoves.size();

	// basically a counting algorithm with carry rippled over.
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
			if ( moves.size() <= startDepth) {
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

		if(stop) return;
		
		if ( ss.back() == final ) {
            gLock.lock();
			if ( solutions.size() < numberOfSolutionsToGet ) {
				solutions.push_back( moves );
                gLock.unlock();
                if ( solutions.size() == numberOfSolutionsToGet ) stop = true;
				goto retard;
			} else {
                gLock.unlock();
                stop = true;
				goto done;
			}
		}
	}
done:
	return;
}

vector<vector<string>> Solver::solveVectors(Puzzle initial, int targetDepth, unsigned int numberOfSolutionsToGet) {
  auto solutions = rawSolve(initial.state, targetDepth, false, numberOfSolutionsToGet);
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

vector<std::string> Solver::solveStrings( Puzzle initial, int depth, unsigned int numberOfSolutionsToGet) {
	stringstream ss;
	vector<std::string> result;
	auto solutions =  solveVectors( initial, depth, numberOfSolutionsToGet);

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

string Solver::solve( Puzzle initial, int depth, unsigned int numberOfSolutionsToGet) {
	stringstream ss;
	auto solutions =  solveVectors( initial, depth, numberOfSolutionsToGet);
	
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



