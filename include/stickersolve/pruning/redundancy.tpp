#pragma once
#include <algorithm> // std::reverse
#include <boost/filesystem.hpp>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <stickersolve/pruning/pruningTree.h>
#include <stickersolve/solver/puzzle.h>
// #include <boost/iostreams/device/mapped_file.hpp>

using namespace std::chrono;

inline void RedundancyTable::generate() {
    int& targetDepth = this->depth;

    if (targetDepth <= 0) return;

    auto start = high_resolution_clock::now();

    cfg->log << "Generating redundancy table (" << path << ") of depth " << this->depth << endl << endl;

    set<vector<uint8_t>> states; // This is a huge memory hog

    vector<int> moves;
    moves.reserve(targetDepth);
    stack<State> ss;
    ss.push(puzzle.solvedState);

    for (size_t i = 0; i < puzzle.solvedState.size(); i++) ss.top()[i] = i;

    states.insert(puzzle.compressState(ss.top()));

    auto validMoves = puzzle.validMoves;
    if (inverse)
        for (auto& move : validMoves) move = !move;
    int numChoices = validMoves.size();

    int depth = 1;
    stats.push_back(0);

    while (depth <= targetDepth) {
        cfg->log << "generating depth: " << depth << endl;
        moves.clear();

        for (;;) {
        advance:
            moves.push_back(0);
            ss.push(ss.top() + validMoves[moves.back()]);

            //if(redundancyTree.contains(moves)) goto retard;


            if ((int)moves.size() >= depth) {
                if (states.count(puzzle.compressState(ss.top()))) {
                    //printMoves(moveNames, moves);
                    stats.back()++;
                    insert(moves);
                } else {
                    states.insert(puzzle.compressState(ss.top()));
                }

                goto retard;
            }
        }

        for (;;) {
        retard:
            ss.pop();
            moves.back()++; // first that doesn't break criteria

            //while( redundancyTree.contains(moves) ) moves.back()++;
            if (moves.back() >= numChoices) {
                moves.pop_back();

                if (moves.size() <= 0) { goto nextDepth; }

                continue;
            }

            ss.push(ss.top() + validMoves[moves.back()]);


            if ((int)moves.size() >= depth && states.count(puzzle.compressState(ss.top()))) {
                //printMoves(moveNames, moves);
                stats.back()++;
                insert(moves);
                goto retard;
            }

            states.insert(puzzle.compressState(ss.top()));

            if ((int)moves.size() < depth) { goto advance; }
        }

    nextDepth:
        stats.push_back(0);
        depth++;
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<chrono::duration<double>>(stop - start);
    cfg->log << "\nRedundancy table (" << path << ") was successfuly generated in " << duration.count() << " seconds\n";
    cfg->log << "----------------------------------------------------------------\n";
}



inline bool RedundancyTable::contains(const vector<int>& e) const {
    auto hash = lastMovesToHash(e);
    return data[hash];
}

inline uint64_t RedundancyTable::lastMovesToHash(const vector<int>& e) const {
    uint64_t hash = 0;

    int i = 0;

    if ((int)e.size() > depth) { i = (int)e.size() - depth; }

    for (; i < (int)e.size(); i++) {
        hash *= puzzle.validMoves.size() + 1;
        hash += e[i] + 1;
    }

    return hash;
}

inline void RedundancyTable::insert(const vector<int>& e) {
    auto hash = lastMovesToHash(e);
    data[hash] = 1;
}

inline void RedundancyTable::unload() {
    if (data != nullptr) {
        cfg->log << "Unloading table (" << path << ") from memory\n";
        cfg->log << "----------------------------------------------------------------\n";
        delete[] data;
        data = nullptr;
    }
}

inline double RedundancyTable::estimateSizeInGb() {
    double actualSize = pow(double(puzzle.validMoves.size() + 1), double(depth)) / 1000000000.0;
    return actualSize;
}

inline void RedundancyTable::performSizeCheck() {
    if (estimateSizeInGb() > cfg->maxMemoryInGb) {
        stringstream ss;
        ss << "error: redundancy table (" << path << ") size exceeds " << cfg->maxMemoryInGb
           << " gigabytes, attempted to allocate " << estimateSizeInGb() << " gigabytes";
        throw runtime_error(ss.str());
    }
}

inline string RedundancyTable::getStats() {
    stringstream ss;
    //uint64_t siz = ( ( uint64_t ) 1 ) << hashSize;
    uint64_t total = 0;
    for (size_t i = 0; i < stats.size(); i++) {
        ss << "Depth " << i + 1 << " has " << stats[i] - total << "." << endl;
        //total = (stats[i]+total)*totalMoves;
    }
    return ss.str();
}

inline void RedundancyTable::load() {
    if (data != nullptr) {
        // 		cfg->log <<  "Skipping loading table (" << path << ") - aleready in memory\n";
        // 		cfg->log <<  "----------------------------------------------------------------\n";
        return; // call unload first
    }

    std::string path = cfg->pruiningTablesPath + "/" + this->path;

    performSizeCheck();

    uint64_t siz = 1;
    // exact power
    for (int i = 0; i < depth; i++) siz *= puzzle.validMoves.size() + 1;

    data = new uint8_t[siz](); // allocate the memory

    if (depth <= 0) {
        cfg->log << "Skipping generating redundancy table (" << path << ")\n";
        cfg->log << "----------------------------------------------------------------\n";
        return;
    }

    if (this->path == "") {
        // create table;
        generate();
        return;
    }

    if (boost::filesystem::exists(path)) {
        // load the table
        cfg->log << "Redundancy table (" << path << ") exists, loading it to memory (size=" << estimateSizeInGb()
                 << "Gb)...\n";
        ifstream infile(path, std::ios::binary);

        uint64_t checksum = 0;
        infile.read((char*)&checksum, sizeof(checksum));

        if (checksum != getChecksum()) {
            throw runtime_error(
                "error: checksum mismatch for redundancy table (" + path +
                ") consider deleting it, or maybe it is from another puzzle?"
            );
        }

        uint64_t statSize = 0;
        infile.read((char*)&statSize, sizeof(uint64_t));
        stats.resize(statSize);
        infile.read((char*)stats.data(), sizeof(stats.data()) * statSize);


        if (!(infile.read((char*)data, siz))) { // read up to the size of the buffer
            if (!infile.eof()) { throw runtime_error("error: redundancy table " + path + " is not of exact length."); }
        }
        //cfg->log <<  "Redundancy table (" + path + ") successfuly loaded.\n";
        cfg->log << "----------------------------------------------------------------\n";
    } else {
        // create table and save it
        puzzle = puzzle.makeUniqueStickers();
        generate();
        boost::filesystem::create_directories(cfg->pruiningTablesPath);
        cfg->log << "Saving table (" << path << ").\n";

        ofstream file(path, std::ios::binary);
        uint64_t checksum = getChecksum();
        file.write((char*)&checksum, sizeof(checksum));

        uint64_t statSize = (uint64_t)stats.size();
        file.write((char*)&statSize, sizeof(uint64_t));
        file.write((char*)stats.data(), sizeof(stats.data()) * stats.size());

        file.write((char*)data, siz);

        cfg->log << "----------------------------------------------------------------\n";
    }
}

inline uint64_t RedundancyTable::getChecksum() {
    const uint64_t goldenRatioConstant = 0x9e3779b97f4a7c15;
    uint64_t result = 0;

    result ^= puzzle.getChecksum();
    result ^= inverse * ((goldenRatioConstant << 6) | (goldenRatioConstant >> (64 - 6)));
    result ^= depth * ((goldenRatioConstant << 40) | (goldenRatioConstant >> (64 - 40)));

    return result;
}
