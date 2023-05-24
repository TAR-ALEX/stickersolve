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
        Puzzle initial,
        int depth,
        bool inverse = false,
        unsigned int numberOfSolutionsToGet = -1
    );
    template <bool removeSymetry = false>
    void generateUniqueStates(
        State initial,
        std::set<State>& states,
        std::deque<std::pair<State, std::vector<int>>>& detach,
        int depth,
        int targetDepth
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
    inline virtual State preInsertTransformation(State s) { return s; }; //TODO: delete this
    virtual bool canDiscardPosition(int movesAvailable, const State& state);
    virtual bool canDiscardMoves(int movesAvailable, const vector<int>& moves);
    virtual void initReverse(){}; // move to public once working
    virtual Puzzle preSolveTransform(Puzzle s1) { return s1; }

public:
    int startMaxDedupDepth = 3;
    std::function<void(int)> progressCallback = [](int) {};
    std::function<void(int)> tableProgressCallback = [](int) {};
    inline virtual void cancel() { terminateEarly = true; }
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

    shared_ptr<estd::thread_safe_queue<string>> asyncIncrementalSolveStrings(
        Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1
    );

    virtual void init(){};
    virtual void deinit(){};
};

class SolverAutoSelector : public Solver {
private:
    void selectSolverCommon(Puzzle initial) {
        estd::raw_ptr<Solver> old = selected;
        selectSolver(initial);
        if (old.get() != selected.get() && old) old->deinit();

        selected->progressCallback = [&](int i) { progressCallback(i); };
        selected->tableProgressCallback = [&](int i) { tableProgressCallback(i); };
    }

public:
    estd::raw_ptr<Solver> selected = nullptr;

    std::function<void(int)> progressCallback = [](int) {};
    std::function<void(int)> tableProgressCallback = [](int) {};
    inline virtual void cancel() { selected->cancel(); }
    virtual void selectSolver(Puzzle initial) = 0;

    shared_ptr<estd::thread_safe_queue<vector<string>>> asyncSolveVectors(
        Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1
    ) {
        selectSolverCommon(initial);
        return selected->asyncSolveVectors(initial, depth, numberOfSolutionsToGet);
    }
    shared_ptr<estd::thread_safe_queue<string>> asyncSolveStrings(
        Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1
    ) {
        selectSolverCommon(initial);
        return selected->asyncSolveStrings(initial, depth, numberOfSolutionsToGet);
    }
    vector<vector<string>> solveVectors(Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1) {
        selectSolverCommon(initial);
        return selected->solveVectors(initial, depth, numberOfSolutionsToGet);
    }
    vector<string> solveStrings(Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1) {
        selectSolverCommon(initial);
        return selected->solveStrings(initial, depth, numberOfSolutionsToGet);
    }
    string solve(Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1) {
        selectSolverCommon(initial);
        return selected->solve(initial, depth, numberOfSolutionsToGet);
    }

    shared_ptr<estd::thread_safe_queue<string>> asyncIncrementalSolveStrings(
        Puzzle initial, int depth, unsigned int numberOfSolutionsToGet = -1
    ) {
        selectSolverCommon(initial);
        return selected->asyncIncrementalSolveStrings(initial, depth, numberOfSolutionsToGet);
    }
};

void printMoves(vector<string>& moveNames, vector<int> moves);