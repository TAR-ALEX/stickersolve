#pragma once

#include <algorithm>
#include <chrono>
#include <estd/string_util.h>
#include <estd/thread_pool.hpp>
#include <sstream>
#include <stack>
#include <stickersolve/solver/PuzzleSolver.h>
#include <stickersolve/solver/puzzle.h>

using namespace std::chrono;
using namespace std;

void Solver::localInit() {}

void Solver::localInitReverse() {
    throw runtime_error(
        string() + "This has not been implemented, remove this exception to get this functionality with bugs related "
                   "to restricted moves, will not work with custom move pruning"
    );
}

void printMoves(vector<string>& moveNames, vector<int> moves) {
    for (auto move : moves) { cout << moveNames[move] << " "; }
    cout << endl;
}

template <bool removeSymetry>
void Solver::generateUniqueStates(
    State initial,
    std::set<State>& states,
    std::deque<std::pair<State, std::vector<int>>>& detach,
    int depth,
    int targetDepth
) {
    states.insert(initial);
    if (detach.empty()) detach.push_back({initial, {}});

    std::vector<State> validMoves = puzzle.validMoves;

    size_t detachOriginalSize = detach.size();

    for (size_t i = 0; i < detachOriginalSize; i++) {
        auto& start = detach.front();
        for (size_t j = 0; j < validMoves.size(); j++) {
            if (terminateEarly) return;
            auto& move = validMoves[j];
            std::vector<int> moves = start.second;
            moves.push_back(j);
            if (canDiscardMoves(targetDepth - depth, moves)) { continue; }
            State end = (start.first + move);
            State trnsfrm;
            if constexpr (removeSymetry) {
                trnsfrm = preInsertTransformation(end);
            } else {
                trnsfrm = end;
            }
            
            if (canDiscardPosition(targetDepth - depth, end)) { continue; }


            if (!states.count(trnsfrm)) {
                states.insert(trnsfrm);
                detach.push_back({end, moves});
            }
        }
        detach.pop_front();
    }
}

void Solver::genLev(
    shared_ptr<estd::thread_safe_queue<vector<int>>> solutions,
    int targetDepth,
    int initialDepth,
    State start,
    vector<int> moves,
    vector<State>& validMoves,
    volatile bool& stop,
    unsigned int numberOfSolutionsToGet
) {
    // if(targetDepth < initialDepth) return;
    State final = puzzle.solvedState;
    moves.reserve(targetDepth);
    stack<State> ss;
    ss.push(start);

    int numChoices = validMoves.size();

    int currentDepth = initialDepth;

    for (;;) {
    advance:
        if (stop) return;
        moves.push_back(0);
        currentDepth++;
        if (canDiscardMoves(targetDepth - currentDepth, moves)) { goto retardNoPop; }
        ss.push(ss.top() + validMoves[moves.back()]);
        if (canDiscardPosition(targetDepth - currentDepth, ss.top())) { goto retard; }
        if (ss.top() == final) {
            if (solutions->numResults() < numberOfSolutionsToGet) {
                solutions->push(moves);
                if (solutions->numResults() >= numberOfSolutionsToGet) stop = true;
            }
        }
        if (currentDepth >= targetDepth) { goto retard; }
        //goto advance;
    }

    for (;;) {
    retard:
        ss.pop();
    retardNoPop:
        moves.back()++; // first that doesn't break criteria

        if (moves.back() >= numChoices) {
            moves.pop_back();
            currentDepth--;

            if (currentDepth <= initialDepth) return;

            continue; // goto retard;
        }
        if (canDiscardMoves(targetDepth - currentDepth, moves)) { goto retardNoPop; }
        ss.push(ss.top() + validMoves[moves.back()]);
        if (canDiscardPosition(targetDepth - currentDepth, ss.top())) { goto retard; }
        if (ss.top() == final) {
            if (solutions->numResults() < numberOfSolutionsToGet) {
                solutions->push(moves);
                if (solutions->numResults() >= numberOfSolutionsToGet) stop = true;
            }
        }

        if (currentDepth < targetDepth) goto advance;
        //goto retard;
    }
}

void Solver::rawSolve(
    shared_ptr<estd::thread_safe_queue<vector<int>>> solutions,
    Puzzle ppzl,
    int targetDepth,
    bool inverse,
    unsigned int numberOfSolutionsToGet
) {
    terminateEarly = false;
    ppzl = preSolveTransform(ppzl);

    puzzle = ppzl;
    // for(auto e: puzzle.getMoves()) std::cout << e << " ";
    // std::cout << "\n";
    // for(auto e: ppzl.getMoves()) std::cout << e << " ";
    // std::cout << "\n";
    // exit(0);

    State initial = puzzle.state;

    if (inverse) {
        localInitReverse();
        initReverse();
    } else {
        localInit();
        init();
    }

    int numChoices = puzzle.validMoves.size();

    std::deque<std::pair<State, std::vector<int>>> detach{};
    std::set<State> visited2{};
    size_t visited2depth = 0;



    cfg->log << "Starting solver with " << cfg->threadPool->getNumThreads() << " threads\n";


    auto start = high_resolution_clock::now();

    vector<int> moves;
    vector<State> ss;
    ss.push_back(initial);
    double memlimMin = 1.0;
    if (cfg->maxMemoryInGb > memlimMin) memlimMin = cfg->maxMemoryInGb;
    while (visited2.size() * numChoices < size_t((memlimMin / 100.0) * 1000000000.0 / 100.0)
    ) { // assume a single scramble is 100 bytes
        if (int(visited2depth) >= targetDepth - 3 && visited2depth != 0) { break; }
        visited2depth++;
        if (numberOfSolutionsToGet == 1) {
            generateUniqueStates<true>(initial, visited2, detach, visited2depth, targetDepth);
        } else {
            generateUniqueStates<false>(initial, visited2, detach, visited2depth, targetDepth);
        }
        cfg->log << "visited2.depth(): " << visited2depth << endl;
        cfg->log << "visited2.size(): " << visited2.size() << endl;
    }

    // cfg->log << "detach depth = " << visited2depth << "\n";
    // cfg->log << "detach width = " << detach.size() << "\n";
    cfg->log << "----------------------------------------------------\n";

    cfg->threadPool->schedule([&]() {
        uint64_t percent = 0;
        long t = 0;
        std::mutex m;
        for (auto& state : detach) {
            if (terminateEarly) break;
            cfg->threadPool->schedule([&]() {
                genLev(
                    solutions,
                    targetDepth,
                    visited2depth,
                    state.first,
                    state.second,
                    puzzle.validMoves,
                    terminateEarly,
                    numberOfSolutionsToGet
                );
            });
            t++;
            if (t * 99 / detach.size() > percent) {
                percent = uint64_t(t * 99.0 / detach.size());
                progressCallback(percent);
            }
        }
    });

    cfg->threadPool->wait();
    progressCallback(100);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<chrono::duration<double>>(stop - start);

    cfg->log << "Solving took: " << duration.count() << " seconds.\n";
    cfg->log << "----------------------------------------------------\n";
    solutions->close();
}

shared_ptr<estd::thread_safe_queue<vector<string>>> Solver::asyncSolveVectors(
    Puzzle initial, int depth, unsigned int numberOfSolutionsToGet
) {
    shared_ptr<estd::thread_safe_queue<vector<int>>> solutions =
        std::make_shared<estd::thread_safe_queue<vector<int>>>();
    shared_ptr<estd::thread_safe_queue<vector<string>>> formattedSolutions =
        std::make_shared<estd::thread_safe_queue<vector<string>>>();
    thread solver([=] { rawSolve(solutions, initial, depth, false, numberOfSolutionsToGet); });
    solver.detach();
    thread converter([=] {
        try {
            while (true) {
                vector<string> formattedSolution;
                vector<int> elements = solutions->pop();
                for (auto moveId : elements) formattedSolution.push_back(puzzle.moveNames[moveId]);
                formattedSolutions->push(formattedSolution);
            }
        } catch (...) {}
        formattedSolutions->close();
    });
    converter.detach();
    return formattedSolutions;
}

shared_ptr<estd::thread_safe_queue<string>> Solver::asyncSolveStrings(
    Puzzle initial, int depth, unsigned int numberOfSolutionsToGet
) {
    shared_ptr<estd::thread_safe_queue<vector<int>>> solutions =
        std::make_shared<estd::thread_safe_queue<vector<int>>>();
    shared_ptr<estd::thread_safe_queue<string>> formattedSolutions =
        std::make_shared<estd::thread_safe_queue<string>>();
    thread solver([=] { rawSolve(solutions, initial, depth, false, numberOfSolutionsToGet); });
    solver.detach();
    thread converter([=] {
        vector<int> elements;
        while (*solutions >> elements) {
            string formattedSolution;
            for (size_t i = 0; i < elements.size(); i++) {
                if (i != 0) formattedSolution += " ";
                formattedSolution += initial.moveNames[elements[i]];
            }
            *formattedSolutions << formattedSolution;
        }
        formattedSolutions->close();
    });
    converter.detach();
    return formattedSolutions;
}

vector<vector<string>> Solver::solveVectors(Puzzle initial, int targetDepth, unsigned int numberOfSolutionsToGet) {
    shared_ptr<estd::thread_safe_queue<vector<int>>> solutionsRaw =
        std::make_shared<estd::thread_safe_queue<vector<int>>>();
    //   auto solutionsQ = asyncSolveVectors(initial, targetDepth, numberOfSolutionsToGet);
    rawSolve(solutionsRaw, initial, targetDepth, false, numberOfSolutionsToGet);
    solutionsRaw->wait(); // not needed
    vector<vector<string>> formattedSolutions;
    vector<int> elements;
    while (*solutionsRaw >> elements) {
        vector<string> formattedSolution;
        for (auto moveId : elements) formattedSolution.push_back(initial.moveNames[moveId]);
        formattedSolutions.push_back(formattedSolution);
    }
    sort(formattedSolutions.begin(), formattedSolutions.end(), [](const vector<string>& a, const vector<string>& b) {
        return a.size() < b.size();
    });
    return formattedSolutions;
}

vector<std::string> Solver::solveStrings(Puzzle initial, int depth, unsigned int numberOfSolutionsToGet) {
    stringstream ss;
    vector<std::string> result;
    auto solutions = solveVectors(initial, depth, numberOfSolutionsToGet);

    for (auto solution : solutions) {
        bool first = true;

        for (auto move : solution) {
            if (!first) {
                ss << " ";
            } else {
                first = false;
            }

            ss << move;
        }

        result.push_back(ss.str());
        ss.clear();
    }

    return result;
}

string Solver::solve(Puzzle initial, int depth, unsigned int numberOfSolutionsToGet) {
    stringstream ss;
    auto solutions = solveVectors(initial, depth, numberOfSolutionsToGet);

    int lastSize = 0;
    for (auto solution : solutions) {
        if ((int)solution.size() > lastSize) {
            lastSize = solution.size();
            ss << "\nSOLUTIONS OF LENGTH " << lastSize << ":\n";
        }

        bool first = true;

        for (auto move : solution) {
            if (!first) {
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

shared_ptr<estd::thread_safe_queue<string>> Solver::asyncIncrementalSolveStrings(
    Puzzle initial, int depth, unsigned int numberOfSolutionsToGet
) {
    std::shared_ptr<estd::thread_safe_queue<std::string>> results{new estd::thread_safe_queue<std::string>()};
    thread solver([=] {
        size_t startDepth = 1;
        while (!terminateEarly) {
            // *results << ("DEPTH " + std::to_string(startDepth));
            auto res = asyncSolveStrings(initial, startDepth, numberOfSolutionsToGet);
            std::string solution;
            while (*res >> solution) {
                if (estd::string_util::splitAll(solution, " ", false).size() == startDepth) *results << solution;
                if (results->numResults() >= numberOfSolutionsToGet) terminateEarly = true;
            }

            startDepth++;
            if (startDepth > (size_t)depth) terminateEarly = true;
        }
        results->close();
        terminateEarly = false;
    });
    solver.detach();
    return results;
}

bool Solver::canDiscardMoves(int movesAvailable, const vector<int>& moves) { return false; }

bool Solver::canDiscardPosition(int movesAvailable, const State& state) { return false; }
