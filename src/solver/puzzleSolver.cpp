#include <stickersolve/solver/puzzle.h>

#include <unistd.h>
#include <stack>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <stickersolve/solver/PuzzleSolver.h>
#include <estd/thread_pool.hpp>

using namespace std::chrono;
using namespace std;


void Solver::localInit() {
	threadManager = estd::thread_pool(cfg->targetThreads);
}

void Solver::localInitReverse() {
	threadManager = estd::thread_pool(cfg->targetThreads);
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

void Solver::rawSolve(shared_ptr<estd::thread_safe_queue<vector<int>>> solutions, State initial, int targetDepth, bool inverse, unsigned int numberOfSolutionsToGet ) {
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

    while(detachWidth < threadManager->getNumThreads()){
        detachWidth *= numChoices;
        detachDepth++;
        if(detachDepth >= targetDepth) {
            break;
        }
    }

    detachDepth++;
    detachWidth *= numChoices;


	log << "Starting solver with "<<threadManager->getNumThreads()<<" threads\n";
	log << "detach depth = "<<detachDepth<<"\n";
	log << "detach width = "<<detachWidth<<"\n";
	log << "----------------------------------------------------\n";

	auto start = high_resolution_clock::now();

    vector<int> moves;
    vector<State> ss;
    ss.push_back(initial);
    volatile bool terminateEarly = false;

	if ( inverse ) {
        rawSolveMultiInverse(ss, targetDepth, detachDepth, moves, solutions, terminateEarly, numberOfSolutionsToGet);
	} else {
        rawSolveMulti(ss, targetDepth, detachDepth, moves, solutions, terminateEarly, numberOfSolutionsToGet);
        threadManager->wait();
	}

	solutions->close();

	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<chrono::duration<double>>( stop - start );

	log << "Solving took: " << duration.count() << " seconds.\n";
	log << "----------------------------------------------------\n";
}


void Solver::rawSolveMultiInverse(
        vector<State> ss,
        int targetDepth,
        int detachDepth,
        vector<int> moves,
        shared_ptr<estd::thread_safe_queue<vector<int>>>& gSolutions,
        volatile bool& stop,
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
	rawSolveMulti(ss, targetDepth, detachDepth, moves, gSolutions, stop, numberOfSolutionsToGet);
	auto inverse = puzzle.buildInverseTable();
    threadManager->wait();

	throw runtime_error( 
		string() + 
		"TODO: implement reverse queue processor" 
	);
	// for(auto& solution : gSolutions){
	// 	// Flip the order
	// 	reverse(solution.begin(),solution.end());
	// 	// Flip the move
	// 	for(int i = 0; i < solution.size(); i++) solution[i] = inverse[solution[i]];
	// }

}

void Solver::rawSolveMulti(
    vector<State> ss,
    int targetDepth,
    int detachDepth,
    vector<int> moves,
    shared_ptr<estd::thread_safe_queue<vector<int>>>& gSolutions,
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
			if ( gSolutions->numResults() < numberOfSolutionsToGet ) {
				gSolutions->push( moves ); // slim chance that you may get more solutions
                stop = true;
				continue;
			} else {
				return;
			}
		}

        if(moves.size() >= detachDepth){
            threadManager->schedule([&gSolutions, &stop, ss, moves, targetDepth, numberOfSolutionsToGet, this] {
                rawSolveRegular(ss, targetDepth, moves.size(), moves, gSolutions, stop, numberOfSolutionsToGet);
            });
        }else{
            rawSolveMulti( ss, targetDepth, detachDepth, moves, gSolutions, stop, numberOfSolutionsToGet);
        }
        moves.back()++;
    }
}

//( vector<State> ss, int targetDepth, int startDepth, vector<int> moves, unsigned int numberOfSolutionsToGet )

void Solver::rawSolveRegular(vector<State> ss, int targetDepth, int startDepth, vector<int> moves, shared_ptr<estd::thread_safe_queue<vector<int>>>& solutions,volatile bool &stop, unsigned int numberOfSolutionsToGet) {
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
			if ( solutions->numResults() < numberOfSolutionsToGet ) {
				solutions->push( moves );
                if ( solutions->numResults() >= numberOfSolutionsToGet ) stop = true;
				goto retard;
			} else {
                stop = true;
				goto done;
			}
		}
	}
done:
	return;
}

shared_ptr<estd::thread_safe_queue<vector<string>>> Solver::asyncSolveVectors( Puzzle initial, int depth, unsigned int numberOfSolutionsToGet){
	shared_ptr<estd::thread_safe_queue<vector<int>>> solutions = std::make_shared<estd::thread_safe_queue<vector<int>>>();
	shared_ptr<estd::thread_safe_queue<vector<string>>> formattedSolutions = std::make_shared<estd::thread_safe_queue<vector<string>>>();
	thread solver(
		[=] {
			rawSolve(solutions, initial.state, depth, false, numberOfSolutionsToGet);
		}
	);
	solver.detach();
	thread converter(
		[=] {
			try{
				while(true){
					vector<string> formattedSolution;
					vector<int> elements = solutions->pop();
					for(auto moveId: elements)
						formattedSolution.push_back(puzzle.moveNames[moveId]);
					formattedSolutions->push(formattedSolution);
				}
			}catch(...){}
			formattedSolutions->close();
		}
	);
	converter.detach();
	return formattedSolutions;
}

shared_ptr<estd::thread_safe_queue<string>> Solver::asyncSolveStrings( Puzzle initial, int depth, unsigned int numberOfSolutionsToGet){
	shared_ptr<estd::thread_safe_queue<vector<int>>> solutions = std::make_shared<estd::thread_safe_queue<vector<int>>>();
	shared_ptr<estd::thread_safe_queue<string>> formattedSolutions = std::make_shared<estd::thread_safe_queue<string>>();
	thread solver(
		[=] {
			rawSolve(solutions, initial.state, depth, false, numberOfSolutionsToGet);
		}
	);
	solver.detach();
	thread converter(
		[=] {
			try{
				while(true){
					string formattedSolution;
					vector<int> elements = solutions->pop();
					for(int i = 0; i < elements.size(); i++){
						if(i != 0) formattedSolution += " ";
						formattedSolution += puzzle.moveNames[elements[i]];
					}
					formattedSolutions->push(formattedSolution);
				}
			}catch(...){}
			formattedSolutions->close();
		}
	);
	converter.detach();
	return formattedSolutions;
}

vector<vector<string>> Solver::solveVectors(Puzzle initial, int targetDepth, unsigned int numberOfSolutionsToGet) {
  shared_ptr<estd::thread_safe_queue<vector<int>>> solutionsRaw = std::make_shared<estd::thread_safe_queue<vector<int>>>();
//   auto solutionsQ = asyncSolveVectors(initial, targetDepth, numberOfSolutionsToGet);
  rawSolve(solutionsRaw, initial.state, targetDepth, false, numberOfSolutionsToGet);
  solutionsRaw->wait(); // not needed
  vector<vector<string>> formattedSolutions;
  vector<int> elements;
  while(*solutionsRaw >> elements){
	vector<string> formattedSolution;
	for(auto moveId: elements)
		formattedSolution.push_back(puzzle.moveNames[moveId]);
	formattedSolutions.push_back(formattedSolution);
  }
  sort(
    formattedSolutions.begin(),
    formattedSolutions.end(),
    [](const vector<string> & a, const vector<string> & b){
      return a.size() < b.size();
    }
  );
  return formattedSolutions;
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



