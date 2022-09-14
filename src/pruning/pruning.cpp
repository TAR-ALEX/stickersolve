#include <stickersolve/pruning/pruningTree.h>
#include <stickersolve/solver/puzzle.h>
#include <algorithm> // std::reverse
#include <boost/filesystem.hpp>
#include <chrono>
#include <estd/ptr.hpp>
#include <map>
#include <stack>
#include <thread>
#include <unistd.h>



using namespace std::chrono;

void PruningStates::generate() {
    if (useMmap)
        throw runtime_error("useMmap flag is not supported with table generation, it is slow and can kill an SSD");

    int& targetDepth = this->depth;

    auto start = high_resolution_clock::now();
    // Build the table incrementally, not currently used, keeping this here
    // for when automatic detection of a full table is implemented
    for (int i = targetDepth; i <= targetDepth; i++) {
        log << "generating depth: " << i << endl;
        generateLevel(i);
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<chrono::duration<double>>(stop - start);
    log << "\nPruning table (" << path << ") was successfuly generated in " << duration.count() << " seconds\n";
    log << "----------------------------------------------------------------\n";
}

void PruningStates::generateLevelSingleThread(
    int targetDepth, int initialDepth, vector<int> moves, vector<State> ss, vector<State> validMoves
) {
    if (targetDepth <= initialDepth) return;

    int numChoices = validMoves.size();

    for (;;) {
        if (moves.size() < targetDepth) {
            moves.push_back(0);
            ss.emplace_back();
        } else {
            moves.back()++;
        }

        while (moves.back() >= numChoices) {
        retard:
            ss.pop_back();
            moves.pop_back();
            if (moves.size() <= initialDepth) { goto done; }
            moves.back()++;
        }

        int movesLeft = targetDepth - moves.size();
        for (;;) {
            if (moves.back() >= numChoices) goto retard;

            if (canDiscardMoves(movesLeft, moves)) {
                moves.back()++;
                continue;
            }

            ss.back() = ss.rbegin()[1] + validMoves[moves.back()];
            break;
        }

        if (ss.back() == puzzle.solvedState) { goto retard; }
        insert(ss.back(), moves.size());
    }
done:
    return;
}

void PruningStates::generateLevelMultiThread(
    int targetDepth, int detachDepth, vector<int> moves, vector<State> ss, vector<State> validMoves
) {
    int numChoices = validMoves.size();

    moves.push_back(0);
    ss.emplace_back();
    int movesLeft = targetDepth - moves.size();
    while (moves.back() < numChoices) {
        if (!canDiscardMoves(movesLeft, moves)) {
            ss.back() = ss.rbegin()[1] + validMoves[moves.back()];
            if (ss.back() == puzzle.solvedState) { continue; }
            insert(ss.back(), moves.size());

            if (moves.size() >= detachDepth) {
                threadManager->schedule([=] {
                    generateLevelSingleThread(targetDepth, moves.size(), moves, ss, validMoves);
                });
            } else {
                generateLevelMultiThread(targetDepth, detachDepth, moves, ss, validMoves);
            }
        }
        moves.back()++;
    }
}


void PruningStates::generateLevel(int lvl) {
    auto& targetDepth = lvl;
    if (targetDepth <= 0) return;

    vector<int> moves;
    moves.reserve(targetDepth);
    vector<State> ss;
    auto solvedState = puzzle.solvedState;
    ss.push_back(solvedState);

    insert(ss.back(), 0);

    auto validMoves = puzzle.validMoves;
    for (auto& move : validMoves) move = !move;

    int numChoices = validMoves.size();

    int detachDepth = 0;
    int detachWidth = 1;

    while (detachWidth < threadManager->getNumThreads()) {
        detachWidth *= numChoices;
        detachDepth++;
        if (detachDepth >= targetDepth) { break; }
    }

    detachDepth++;
    detachWidth *= numChoices;

    if (detachDepth >= targetDepth || threadManager->getNumThreads() == 1) {
        generateLevelSingleThread(targetDepth, moves.size(), moves, ss, validMoves);
        return;
    }

    log << "targetThreads: " << threadManager->getNumThreads() << endl;
    log << "detachWidth: " << detachWidth << endl;
    log << "detachDepth: " << detachDepth << endl;


    generateLevelMultiThread(targetDepth, detachDepth, moves, ss, validMoves);


    threadManager->wait();
}

bool PruningStates::canDiscardMoves(int movesAvailable, const vector<int>& moves) {
    return redundancyTableInverse.contains(moves);
}
