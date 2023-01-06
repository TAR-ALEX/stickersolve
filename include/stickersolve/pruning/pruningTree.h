#pragma once

#include <boost/iostreams/device/mapped_file.hpp>
#include <estd/ostream_proxy.hpp>
#include <estd/ptr.hpp>
#include <estd/thread_pool.hpp>
#include <iostream>
#include <map>
#include <mutex>
#include <set>
#include <stickersolve/config.hpp>
#include <stickersolve/pruning/redundancy.hpp>
#include <stickersolve/solver/puzzle.h>
#include <stickersolve/solver/puzzleState.h>
#include <vector>

class Puzzle;

using namespace std;

template <int width = 2> // width in nibbles, must be a multiple of 4 (2 IS A SINGLE BYTE)
class PruningStates {
private:
    RedundancyTable redundancyTableInverse;
    // using a mutex to access the table ended up having very poor performance, the chances of race conditions are close to zero
    // mutex tableMutex;
    boost::iostreams::mapped_file file;
    void performSizeCheck();
    int visited2depth;
    // std::map<State, uint8_t> visited2;
    std::set<State> visited2;
    void generate();
    void generateLevel(int lvl);
    void genLev(
        int targetDepth, int initialDepth, State state, vector<int> moves, vector<State>& validMoves
    );
    void generateUniqueStates(std::set<State>& dups, std::deque<std::pair<State, std::vector<int>>>& detach, int depth);
    uint64_t discardBits = 64;
    void initHashMask();
    uint8_t* data = nullptr;
    virtual bool canDiscardMoves(int movesAvailable, const vector<int>& moves);
    inline virtual State preInsertTransformation(State s) { return s; };
    inline virtual State preLookupTransformation(State s) { return s; };
    vector<uint64_t> stats = vector<uint64_t>(256, 0);

public:
    uint64_t getChecksum();
    double estimateSizeInGb();

    virtual ~PruningStates() { unload(); }

    int redundancyInverseDepth = 5;
    Puzzle puzzle;
    int hashSize = 16;
    int depth = 0;

    string path = "";
    estd::joint_ptr<SolverConfig> cfg;

    void load();   // consumes a lot of memory
    void unload(); // frees the memory.

    int cannotBeSolvedInLimit(int movesAvailable, State hash);
    uint8_t getDistance(State hash);
    inline bool cannotUseTable(int movesAvailable) { return depth < movesAvailable; }
    inline bool canUseTable(int movesAvailable) { return !cannotUseTable(movesAvailable); }

    virtual void insert(State hash, int moves);

    string getStats();
};

#include <stickersolve/pruning/pruningTree.tpp>