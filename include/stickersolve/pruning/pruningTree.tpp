#pragma once
#include <algorithm> // std::reverse
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <chrono>
#include <cmath>
#include <estd/ptr.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <stickersolve/pruning/pruningTree.h>
#include <stickersolve/solver/puzzle.h>
#include <thread>
#include <estd/hugepage.hpp>

using namespace std::chrono;
using namespace boost::iostreams;

template <class Puzzle, int width, bool useSym, bool useInverse>
double PruningStates<Puzzle, width, useSym, useInverse>::estimateSizeInGb() {
    uint64_t siz = ((uint64_t)1) << hashSize;
    if constexpr (width == 1) { siz >>= 1; }
    if constexpr (width == 0) { siz >>= 3; }
    double actualSize = double(siz) / 1000000000.0;
    return actualSize;
}


template <class Puzzle, int width, bool useSym, bool useInverse>
void PruningStates<Puzzle, width, useSym, useInverse>::performSizeCheck() {
    if (estimateSizeInGb() > cfg->maxMemoryInGb) {
        stringstream ss;
        ss << "Pruning table size exceeds " << cfg->maxMemoryInGb << " gigabytes, attempted to allocate "
           << estimateSizeInGb() << " gigabytes";
        throw runtime_error(ss.str());
    }
}


template <class Puzzle, int width, bool useSym, bool useInverse>
void PruningStates<Puzzle, width, useSym, useInverse>::insert(State state, int moves) {
    if constexpr (width == 2) {
        auto hash = state.toHash() >> discardBits;
        //tableMutex.lock();
        ((volatile uint8_t&)data[hash]) = data[hash] < moves ? data[hash] : moves;
        //tableMutex.unlock();
    } else if constexpr (width == 1) {
        auto hash = state.toHash() >> discardBits;
        uint8_t val = data[hash >> 1];
        uint8_t left = val >> 4;
        uint8_t right = val & 0xF;
        if ((hash & 1) && moves < left) {
            ((volatile uint8_t&)data[hash >> 1]) = (moves << 4) | right;
        } else if (!(hash & 1) && moves < right) {
            ((volatile uint8_t&)data[hash >> 1]) = (left << 4) | moves;
        }
    } else if constexpr (width == 0) {
        auto hash = state.toHash() >> discardBits;
        auto idx = hash >> 3;
        auto bitNum = hash & 0b111; // 111 binary
        auto bitMask = 1 << bitNum;
        uint8_t val = data[idx];

        ((volatile uint8_t&)data[idx]) = val | bitMask;
    }
}


template <class Puzzle, int width, bool useSym, bool useInverse>
int PruningStates<Puzzle, width, useSym, useInverse>::cannotBeSolvedInLimit(int movesAvailable, State state) {
    if (depth >= movesAvailable) {
        if constexpr (width == 2) {
            auto hash = state.toHash() >> discardBits;
            if (data[hash] > movesAvailable) return 1;
        } else if constexpr (width == 1) {
            auto hash = state.toHash() >> discardBits;
            uint8_t val = data[hash >> 1];
            if (hash & 1) val = val >> 4;
            val &= 0x0F;
            if (val > movesAvailable) return 1;
        } else if constexpr (width == 0) {
            auto hash = state.toHash() >> discardBits;
            auto idx = hash >> 3;
            auto bitNum = hash & 0b111; // 111 binary
            auto bitMask = 1 << bitNum;
            if ((data[idx] & bitMask) == 0) return 1; // if not in the table we cannot solve it
        }
        return -1;
    }
    return 0; // cant use table
}


template <class Puzzle, int width, bool useSym, bool useInverse>
uint8_t PruningStates<Puzzle, width, useSym, useInverse>::getDistance(State state) {
    if constexpr (width == 2) {
        auto hash = state.toHash() >> discardBits;
        return data[hash];
    } else if constexpr (width == 1) {
        auto hash = state.toHash() >> discardBits;
        uint8_t val = data[hash >> 1];
        if (hash & 1) val = val >> 4;
        val &= 0x0F;
        return val;
    } else if constexpr (width == 0) {
        auto hash = state.toHash() >> discardBits;
        auto idx = hash >> 3;
        auto bitNum = hash & 0b111; // 111 binary
        auto bitMask = 1 << bitNum;
        return (data[idx] & bitMask) ? (depth) : (depth + 1);
    } else {
        return 0;
    }
}


template <class Puzzle, int width, bool useSym, bool useInverse>
void PruningStates<Puzzle, width, useSym, useInverse>::unload() {
    if (data != nullptr) {
        cfg->log << "Unloading table (" << path << ") from memory\n";
        cfg->log << "----------------------------------------------------------------\n";

        free(data);

        data = nullptr;
        progressCallback(0);
    }
}


template <class Puzzle, int width, bool useSym, bool useInverse>
void PruningStates<Puzzle, width, useSym, useInverse>::load() {
    if(cfg->overrideDepth > 0) this->depth = cfg->overrideDepth;
    terminateEarly = false;
    progressCallback(0);
    if (data != nullptr) { 
        progressCallback(100);
        return; 
    }

    std::string path = cfg->pruiningTablesPath + "/" + this->path;
    if (this->path == "") path = "";

    performSizeCheck();
    initHashMask();
    uint64_t siz = ((uint64_t)1) << hashSize;
    if constexpr (width == 1) { siz >>= 1; }
    if constexpr (width == 0) { siz >>= 3; }

    if (this->path == "" && cfg->useMmapForPruning) {
        throw runtime_error("error: cannot have use a memory mapped file without specifying a file name or path.");
    }

    if (this->path != "" && boost::filesystem::exists(path)) {
        if (!cfg->useMmapForPruning) {
            cfg->log << "Pruning table (" << path << ") exists, loading it to memory (size=" << estimateSizeInGb()
                     << "Gb)...\n";

            data = reinterpret_cast<uint8_t*>(hugepage_malloc(siz));

            uint64_t checksum = 0;
            ifstream infile(path, std::ios::binary);

            infile.read((char*)&checksum, sizeof(checksum));
            if (checksum != getChecksum()) {
                throw runtime_error(
                    "error: checksum mismatch for pruning table (" + path +
                    ") consider deleting it, or maybe it is from another puzzle?"
                );
            }
            infile.read((char*)stats.data(), sizeof(stats.data()) * stats.size());

            progressCallback(0);
            for (int percent = 0; percent < 100; percent++) {
                size_t lastLoadedSize = siz*(percent)/100;
                size_t loadedSize = siz*(percent+1)/100;
                if (!(infile.read((char*)data+lastLoadedSize, loadedSize-lastLoadedSize))) { 
                    if (!infile.eof()) {
                        stringstream ss;
                        ss << "error: table (" << path << ") is not of the exact length.";
                        throw runtime_error(ss.str());
                    }
                }
                progressCallback(percent+1);
            }

            // if (!(infile.read((char*)data, siz))) { // read up to the size of the buffer
            //     if (!infile.eof()) {
            //         stringstream ss;
            //         ss << "error: table (" << path << ") is not of the exact length.";
            //         throw runtime_error(ss.str());
            //     }
            // }
        } else {
            cfg->log << "Pruning table (" << path
                     << ") exists, opening it as a memory mapped file (size=" << estimateSizeInGb() << "Gb)...\n";
            mapped_file_params params;
            params.path = path;
            params.length = siz;
            params.flags = mapped_file::mapmode::readwrite;

            file.open(params);

            if (!file.is_open()) {
                stringstream ss;
                ss << "error: could not open table (" << path << ") as a memory mapped file.";
                throw runtime_error(ss.str());
            }

            data = static_cast<uint8_t*>((void*)file.data());

            uint64_t checksum = 0;
            checksum = *((uint64_t*)data);
            data += sizeof(checksum);
            uint64_t* statsPtr = (uint64_t*)data;
            data += sizeof(stats.data()) * stats.size();

            for (size_t i = 0; i < stats.size(); i++) { stats[i] = statsPtr[i]; }

            if (checksum != getChecksum()) {
                throw runtime_error(
                    "error: checksum mismatch for pruning table (" + path +
                    ") consider deleting it, or maybe it is from another puzzle?"
                );
            }
        }
    } else {
        redundancyTableInverse.puzzle = this->puzzle.makeUniqueStickers();// TODO: for 3 color, it must not be the 3 color puzzle, all stickers must be unique.
        redundancyTableInverse.cfg = cfg;
        redundancyTableInverse.inverse = true;
        redundancyTableInverse.depth = redundancyInverseDepth;
        redundancyTableInverse.load();
        if (cfg->useMmapForPruning) {
            uint64_t checksum = getChecksum();
            cfg->log << "Creating pruning table (" << path << ") of size " << estimateSizeInGb()
                     << "Gb as a memory mapped file...\n\n";
            mapped_file_params params;
            params.path = path;
            params.new_file_size = siz + sizeof(checksum);
            params.length = siz;
            params.flags = mapped_file::mapmode::readwrite;

            file.open(params);

            if (!file.is_open()) {
                stringstream ss;
                ss << "error: could not create table (" << path << ") as a memory mapped file.";
                throw runtime_error(ss.str());
            }

            data = static_cast<uint8_t*>((void*)file.data());

            ((uint64_t*)data)[0] = checksum;
            data += sizeof(checksum);
        } else {
            cfg->log << "Creating pruning table (" << path << ") of size " << estimateSizeInGb()
                     << " Gb in memory...\n\n";
            data = reinterpret_cast<uint8_t*>(hugepage_malloc(siz));
        }

        for (uint64_t i = 0; i < siz; i++) {
            if constexpr (width >= 2) {
                data[i] = depth + 1;
            } else if constexpr (width == 1) {
                data[i] = (depth + 1) | ((depth + 1) << 4);
            } else if constexpr (width == 0) {
                data[i] = 0;
            }
        }

        generate();

        redundancyTableInverse.unload();
        visited2depth = 0;
        visited2.clear();

        if(terminateEarly){
            this->progressCallback(100);
            unload();
            // terminateEarly = false;
        }else if (!cfg->useMmapForPruning && this->path != "") {
            boost::filesystem::create_directories(cfg->pruiningTablesPath);
            cfg->log << "\nSaving table (" << path << ").\n";
            ofstream file(path, std::ios::binary);
            uint64_t checksum = getChecksum();
            file.write((char*)&checksum, sizeof(checksum));
            file.write((char*)stats.data(), sizeof(stats.data()) * stats.size());
            file.write((char*)data, siz);
            file.close();
        }
        this->progressCallback(100);
    }

    cfg->log << "----------------------------------------------------------------\n";
}


template <class Puzzle, int width, bool useSym, bool useInverse>
string PruningStates<Puzzle, width, useSym, useInverse>::getStats() {
    std::string path = cfg->pruiningTablesPath + "/" + this->path;
    if (this->path == "") path = "";

    uint64_t siz = ((uint64_t)1) << hashSize;
    stringstream ss;
    ss << "maxDepth = " << depth << endl;

    bool needsStats = true;

    for (int i = 0; i < 256; i++)
        if (stats[i]) needsStats = false;

    if (needsStats) {
        if constexpr (width == 2) {
            for (uint64_t i = 0; i < siz; i++) { stats[data[i]]++; } //count actual stats
        } else if constexpr (width == 1) {
            for (uint64_t i = 0; i < siz >> 1; i++) {
                stats[data[i] >> 4]++;
                stats[data[i] & 0xF]++;
            }
        } else if constexpr (width == 0) {
            for (uint64_t i = 0; i < siz >> 3; i++) {
                for (int bit = 0; bit < 8; bit++) stats[(data[i] >> bit) & 1]++;
            }
        }
        fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        uint64_t checksum = getChecksum();
        file.write((char*)&checksum, sizeof(checksum));
        file.write((char*)stats.data(), sizeof(stats.data()) * stats.size());
        file.close();
    }

    uint64_t sum = 0;
    for (int i = 0; i <= depth; i++) {
        if (stats[i]) ss << "Depth " << i << " has " << stats[i] << "." << endl;
        if constexpr (width == 0) {
            if (i == 1) sum += stats[i];
        } else {
            sum += stats[i];
        }
    }
    if (stats[depth + 1]) ss << "Depth " << depth + 1 << " has " << stats[depth + 1] << "." << endl;

    double spaceInTable = double(sum) / 1000000000.0;
    if constexpr (width == 1) spaceInTable /= 2;
    if constexpr (width == 0) spaceInTable /= 8;

    ss << spaceInTable << " Gb / " << estimateSizeInGb() << " Gb = " << spaceInTable / estimateSizeInGb() << endl;

    return ss.str();
}


template <class Puzzle, int width, bool useSym, bool useInverse>
void PruningStates<Puzzle, width, useSym, useInverse>::initHashMask() {
    discardBits = 64 - hashSize;
}


template <class Puzzle, int width, bool useSym, bool useInverse>
uint64_t PruningStates<Puzzle, width, useSym, useInverse>::getChecksum() {
    const uint64_t goldenRatioConstant = 0x9e3779b97f4a7c15;
    uint64_t result = 0;

    result ^= puzzle.getChecksum();
    result ^= hashSize * ((goldenRatioConstant << 6) | (goldenRatioConstant >> (64 - 6)));
    //result ^= width * ((goldenRatioConstant << 20) | (goldenRatioConstant >> (64 - 20)));
    result ^= depth * ((goldenRatioConstant << 40) | (goldenRatioConstant >> (64 - 40)));

    return result;
}



template <class Puzzle, int width, bool useSym, bool useInverse>
void PruningStates<Puzzle, width, useSym, useInverse>::generate() {
    if (cfg->useMmapForPruning)
        throw runtime_error(
            "useMmapForPruning flag is not supported with table generation, it is slow and can kill an SSD"
        );

    int& targetDepth = this->depth;

    auto start = high_resolution_clock::now();
    // Build the table incrementally, not currently used, keeping this here
    // for when automatic detection of a full table is implemented
    for (int i = targetDepth; i <= targetDepth; i++) {
        cfg->log << "generating depth: " << i << endl;
        generateLevel(i);
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<chrono::duration<double>>(stop - start);
    cfg->log << "\nPruning table (" << path << ") was successfuly generated in " << duration.count() << " seconds\n";
    cfg->log << "----------------------------------------------------------------\n";
}

template <class Puzzle, int width, bool useSym, bool useInverse>
void PruningStates<Puzzle, width, useSym, useInverse>::generateUniqueStates(
    std::set<State>& states, std::deque<std::pair<State, std::vector<int>>>& detach, int depth
) {
    states.insert(puzzle.solvedState);
    insert(puzzle.solvedState, 0);
    if (detach.empty()) detach.push_back({puzzle.solvedState, {}});

    std::vector<State> validMoves = puzzle.validMoves;
    for (auto& move : validMoves) move = !move;

    size_t detachOriginalSize = detach.size();

    for (size_t i = 0; i < detachOriginalSize; i++) {
        auto& start = detach.front();
        for (size_t j = 0; j < validMoves.size(); j++) {
            auto& move = validMoves[j];
            State end = (start.first + move); //preInsertTransformation
            //State trnsfrm = preInsertTransformation(end);
            State trnsfrm;
            if constexpr(useSym) {
                trnsfrm = puzzle.getUniqueSymetric(end); // we cant just use preInsertTransformation as we wont cover all states
            }else{
                trnsfrm = end;
            }


            if (!states.count(trnsfrm)) {
                std::vector<int> moves = start.second;
                moves.push_back(j);
                //if (canDiscardMoves(depth, moves)) { continue; }
                states.insert(trnsfrm);
                
                
                detach.push_back({end, moves});
                if constexpr (useInverse){
                    State inverse = puzzle.getUniqueSymetric(!end); 
                    if (inverse < trnsfrm) trnsfrm = inverse; // getUniqueSymetricInverse equivalent
                }
                insert(trnsfrm, depth);
            }
        }
        detach.pop_front();
    }
}

template <class Puzzle, int width, bool useSym, bool useInverse>
void PruningStates<Puzzle, width, useSym, useInverse>::genLev(
    int targetDepth, int initialDepth, State start, vector<int> moves, vector<State>& validMoves
) {
    moves.reserve(targetDepth);
    stack<State> ss;
    ss.push(start);

    int numChoices = validMoves.size();

    int currentDepth = initialDepth;
    State transformedState = start;

    if (terminateEarly) return;

    for (;;) {
    advance:
        moves.push_back(0);
        currentDepth++;
        if (canDiscardMoves(targetDepth - currentDepth, moves)) { goto retardNoPop; }
        ss.push(ss.top() + validMoves[moves.back()]);
        transformedState = preInsertTransformation(ss.top());
        insert(transformedState, currentDepth);
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
        transformedState = preInsertTransformation(ss.top());
        //if (visited2.count(transformedState)) continue; // goto retard
        insert(transformedState, currentDepth);

        if (currentDepth < targetDepth) goto advance;
        //goto retard;
    }
}

template <class Puzzle, int width, bool useSym, bool useInverse>
void PruningStates<Puzzle, width, useSym, useInverse>::generateLevel(int lvl) {
    auto& targetDepth = lvl;
    if (targetDepth <= 0) return;

    vector<int> moves;
    moves.reserve(targetDepth);
    vector<State> ss;
    auto solvedState = puzzle.solvedState;
    ss.push_back(solvedState);

    insert(preInsertTransformation(ss.back()), 0);

    auto validMoves = puzzle.validMoves;
    for (auto& move : validMoves) move = !move;

    int numChoices = validMoves.size();

    visited2depth = 0;


    std::deque<std::pair<State, std::vector<int>>> detach{};
    while (visited2.size() * numChoices < size_t((cfg->maxMemoryInGb / 100.0) * 1000000000.0 / 100.0)
    ) { // assume a single scramble is 100 bytes
        if (visited2depth >= targetDepth - 3) { break; }
        visited2depth++;
        generateUniqueStates(visited2, detach, visited2depth);
        cfg->log << "visited2.depth(): " << visited2depth << endl;
        cfg->log << "visited2.size(): " << visited2.size() << endl;
    }

    cfg->threadPool->schedule([&]() {
        uint64_t percent = 0;
        long t = 0;
        for (auto& state : detach) {
            cfg->threadPool->schedule([&]() {
                genLev(targetDepth, visited2depth, state.first, state.second, validMoves);
            });
            t++;
            if (t * 99 / detach.size() > percent) {
                percent = t * 99 / detach.size();
                this->progressCallback(percent);
                // cfg->log << percent << "%\n";
                // cfg->log << t << "\n";
                // cfg->log << "genLev(" << targetDepth << ", " << visited2depth << ")\n";
            }
        }
        cfg->log << "done\n";
    });

    cfg->threadPool->wait();
}

template <class Puzzle, int width, bool useSym, bool useInverse>
bool PruningStates<Puzzle, width, useSym, useInverse>::canDiscardMoves(int movesAvailable, const vector<int>& moves) {
    return redundancyTableInverse.contains(moves);
}
