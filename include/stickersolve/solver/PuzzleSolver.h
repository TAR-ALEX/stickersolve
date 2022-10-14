#pragma once

#include <estd/ostream_proxy.hpp>
#include <estd/ptr.hpp>
#include <estd/thread_pool.hpp>
#include <estd/thread_safe_queue.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <stickersolve/config.hpp>
#include <stickersolve/pruning/pruningTree.h>
#include <stickersolve/pruning/redundancy.hpp>
#include <stickersolve/solver/puzzle.h>
#include <stickersolve/solver/puzzleState.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

class Solver {
private:
    int numStickers = 1;

    void calculateStickerWidth();

    virtual void localInit();
    virtual void localInitReverse();

    void rawSolveMulti(
        vector<State> ss,
        int targetDepth,
        int detachDepth,
        vector<int> moves,
        shared_ptr<estd::thread_safe_queue<vector<int>>>& gSolutions,
        volatile bool& stop,
        unsigned int numberOfSolutionsToGet = -1
    );

    void rawSolveRegular(
        vector<State> ss,
        int depth,
        int startDepth,
        vector<int> moves,
        shared_ptr<estd::thread_safe_queue<vector<int>>>& gSolutions,
        volatile bool& stop,
        unsigned int numberOfSolutionsToGet = -1
    );

    void rawSolve(
        shared_ptr<estd::thread_safe_queue<vector<int>>>,
        State initial,
        int depth,
        bool inverse = false,
        unsigned int numberOfSolutionsToGet = -1
    );

    void rawSolveMultiInverse(
        vector<State> ss,
        int targetDepth,
        int detachDepth,
        vector<int> moves,
        shared_ptr<estd::thread_safe_queue<vector<int>>>& gSolutions,
        volatile bool& stop,
        unsigned int numberOfSolutionsToGet = -1
    );

protected:
    virtual bool canDiscardPosition(int movesAvailable, const State& state);
    virtual bool canDiscardMoves(int movesAvailable, const vector<int>& moves);
    Puzzle puzzle;
    virtual void initReverse(){}; // move to public once working
    virtual State preSolveTransform(State s1) { return s1; }

public:
    estd::joint_ptr<SolverConfig> cfg = new SolverConfig();

    shared_ptr<estd::thread_safe_queue<vector<string>>> asyncSolveVectors(
        Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1
    );
    shared_ptr<estd::thread_safe_queue<string>> asyncSolveStrings(
        Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1
    );

    vector<vector<string>> solveVectors(Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1);
    vector<string> solveStrings(Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1);
    string solve(Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1);

    virtual void init(){};
};

void printMoves(vector<string>& moveNames, vector<int> moves);

#include <stickersolve/solver/puzzleSolver.tpp>