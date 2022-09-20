#pragma once

#include <boost/iostreams/device/mapped_file.hpp>
#include <estd/ostream_proxy.hpp>
#include <iostream>
#include <map>
#include <set>
#include <stickersolve/config.hpp>
#include <stickersolve/solver/puzzle.h>
#include <stickersolve/solver/puzzleState.h>
#include <vector>

class Puzzle;

class RedundancyTable {
private:
    uint64_t lastMovesToHash(const vector<int>& e) const;

    void performSizeCheck();
    void generate();

public:
    virtual ~RedundancyTable() { unload(); }

    uint64_t getChecksum();
    vector<uint64_t> stats;
    uint8_t* data = nullptr;
    string path = "";
    int depth = 0;
    bool inverse = false;
    Puzzle puzzle;
    estd::joint_ptr<SolverConfig> cfg;

    double estimateSizeInGb();

    bool contains(const vector<int>& e) const;
    void insert(const vector<int>& e);

    void load();   // consumes a lot of memory
    void unload(); // frees the memory.

    string getStats();
};
