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
    volatile bool terminateEarly = false;
    int numStickers = 1;

    void calculateStickerWidth();

    void localInit();
    void localInitReverse();

    void rawSolve(
        shared_ptr<estd::thread_safe_queue<vector<int>>>,
        State initial,
        int depth,
        bool inverse = false,
        unsigned int numberOfSolutionsToGet = -1
    );
    template<bool removeSymetry = false>
    void generateUniqueStates(
        State initial, std::set<State>& states, std::deque<std::pair<State, std::vector<int>>>& detach, int depth, int targetDepth
    );
    void genLev(
        shared_ptr<estd::thread_safe_queue<vector<int>>> solutions,
        int targetDepth,
        int initialDepth,
        State state,
        vector<int> moves,
        vector<State>& validMoves,
        volatile bool& stop,
        unsigned int numberOfSolutionsToGet = -1
    );

protected:
    Puzzle puzzle; // reference puzzle that the solver is capable of solving, only used for getting the moves
    inline virtual State preInsertTransformation(State s) { return s; };//TODO: delete this
    virtual bool canDiscardPosition(int movesAvailable, const State& state);
    virtual bool canDiscardMoves(int movesAvailable, const vector<int>& moves);
    virtual void initReverse(){}; // move to public once working
    virtual State preSolveTransform(State s1) { return s1; }

public:
    std::function<void(int)> progressCallback = [](int){};
    std::function<void(int)> tableProgressCallback = [](int){};
    inline void cancel() {terminateEarly = true;}
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