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

using namespace std::chrono;
using namespace boost::iostreams;

template <int width>
double PruningStates<width>::estimateSizeInGb() {
    uint64_t siz = ((uint64_t)1) << hashSize;
    if constexpr (width == 1) { siz >>= 1; }
    if constexpr (width == 0) { siz >>= 3; }
    double actualSize = double(siz) / 1000000000.0;
    return actualSize;
}


template <int width>
void PruningStates<width>::performSizeCheck() {
    if (estimateSizeInGb() > cfg->maxMemoryInGb) {
        stringstream ss;
        ss << "Pruning table size exceeds " << cfg->maxMemoryInGb << " gigabytes, attempted to allocate "
           << estimateSizeInGb() << " gigabytes";
        throw runtime_error(ss.str());
    }
}


template <int width>
void PruningStates<width>::insert(State state, int moves) {
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


template <int width>
bool PruningStates<width>::cannotBeSolvedInLimit(int movesAvailable, State state) {
    state = preLookupTransformation(state);
    if constexpr (width == 2) {
        if (depth >= movesAvailable) {
            auto hash = state.toHash() >> discardBits;
            return data[hash] > movesAvailable;
        }
    } else if constexpr (width == 1) {
        if (depth >= movesAvailable) {
            auto hash = state.toHash() >> discardBits;
            uint8_t val = data[hash >> 1];
            if (hash & 1) val = val >> 4;
            val &= 0x0F;
            return val > movesAvailable;
        }
    } else if constexpr (width == 0) {
        if (depth >= movesAvailable) {
            auto hash = state.toHash() >> discardBits;
            auto idx = hash >> 3;
            auto bitNum = hash & 0b111; // 111 binary
            auto bitMask = 1 << bitNum;
            return (data[idx] & bitMask) == 0; // if not in the table we cannot solve it
        }
    }
    return false;
}


template <int width>
uint8_t PruningStates<width>::getDistance(State state) {
    state = preLookupTransformation(state);
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


template <int width>
void PruningStates<width>::unload() {
    if (data != nullptr) {
        cfg->log << "Unloading table (" << path << ") from memory\n";
        cfg->log << "----------------------------------------------------------------\n";

        delete[] data;

        data = nullptr;
    }
}


template <int width>
void PruningStates<width>::load() {
    if (data != nullptr) { return; }

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

            data = new uint8_t[siz];

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


            if (!(infile.read((char*)data, siz))) { // read up to the size of the buffer
                if (!infile.eof()) {
                    stringstream ss;
                    ss << "error: table (" << path << ") is not of the exact length.";
                    throw runtime_error(ss.str());
                }
            }
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
        redundancyTableInverse.puzzle = this->puzzle;
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
            data = new uint8_t[siz];
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

        if (!cfg->useMmapForPruning && this->path != "") {
            cfg->log << "\nSaving table (" << path << ").\n";
            ofstream file(path, std::ios::binary);
            uint64_t checksum = getChecksum();
            file.write((char*)&checksum, sizeof(checksum));
            file.write((char*)stats.data(), sizeof(stats.data()) * stats.size());
            file.write((char*)data, siz);
            file.close();
        }
    }

    cfg->log << "----------------------------------------------------------------\n";
}


template <int width>
string PruningStates<width>::getStats() {
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


template <int width>
void PruningStates<width>::initHashMask() {
    discardBits = 64 - hashSize;
}


template <int width>
uint64_t PruningStates<width>::getChecksum() {
    const uint64_t goldenRatioConstant = 0x9e3779b97f4a7c15;
    uint64_t result = 0;

    result ^= puzzle.getChecksum();
    result ^= hashSize * ((goldenRatioConstant << 6) | (goldenRatioConstant >> (64 - 6)));
    //result ^= width * ((goldenRatioConstant << 20) | (goldenRatioConstant >> (64 - 20)));
    result ^= depth * ((goldenRatioConstant << 40) | (goldenRatioConstant >> (64 - 40)));

    return result;
}



template <int width>
void PruningStates<width>::generate() {
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

template <int width>
void PruningStates<width>::generateLevelSingleThread(
    int targetDepth, int initialDepth, vector<int> moves, vector<State> ss, vector<State> validMoves
) {
    if (targetDepth <= initialDepth) return;

    int numChoices = validMoves.size();

    for (;;) {
        if ((int)moves.size() < targetDepth) {
            moves.push_back(0);
            ss.emplace_back();
        } else {
            moves.back()++;
        }

        while ((int)moves.back() >= numChoices) {
        retard:
            ss.pop_back();
            moves.pop_back();
            if ((int)moves.size() <= initialDepth) { goto done; }
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
            // if (ss.back() == puzzle.solvedState) {
            //     moves.back()++;
            //     continue;
            // }

            break;
        }

        //State s1 = preInsertTransformation(ss.back());
        insert(preInsertTransformation(ss.back()), moves.size());
        // if (checkVisited(s1)) {
        //     moves.back()++;
        //     goto advance;
        // }
    }
done:
    return;
}

template <int width>
void PruningStates<width>::generateLevelMultiThread(
    int targetDepth, int detachDepth, vector<int> moves, vector<State> ss, vector<State> validMoves
) {
    int numChoices = validMoves.size();

    moves.push_back(0);
    ss.emplace_back();
    int movesLeft = targetDepth - moves.size();
    while (moves.back() < numChoices) {
        if (!canDiscardMoves(movesLeft, moves)) {
            ss.back() = ss.rbegin()[1] + validMoves[moves.back()];
            if (ss.back() == puzzle.solvedState) { // is this needed? solved state can be stored as depth zero
                moves.back()++;
                continue;
            }
            State trnsfrm = preInsertTransformation(ss.back());
            insert(trnsfrm, moves.size());
            if (checkVisited(trnsfrm, moves.size())) {
                moves.back()++;
                continue;
            }

            if ((int)moves.size() >= detachDepth) {
                cfg->threadPool->schedule([=] {
                    generateLevelSingleThread(targetDepth, moves.size(), moves, ss, validMoves);
                });
            } else {
                generateLevelMultiThread(targetDepth, detachDepth, moves, ss, validMoves);
            }
        }
        moves.back()++;
    }
}

// template <int width>
// std::set<std::vector<uint8_t>> PruningStates<width>::generateUniqueStates(
//     int targetDepth, int detachDepth, vector<int> moves, vector<State> ss, vector<State> validMoves
// ) {
//     int numChoices = validMoves.size();

//     moves.push_back(0);
//     ss.emplace_back();
//     int movesLeft = targetDepth - moves.size();
//     while (moves.back() < numChoices) {
//         if (!canDiscardMoves(movesLeft, moves)) {
//             ss.back() = ss.rbegin()[1] + validMoves[moves.back()];
//             if (ss.back() == puzzle.solvedState) { // is this needed? solved state can be stored as depth zero
//                 moves.back()++;
//                 continue;
//             }
//             State trnsfrm = preInsertTransformation(ss.back());
//             if (checkVisited(trnsfrm, moves.size())) {
//                 moves.back()++;
//                 continue;
//             }
//             insert(trnsfrm, moves.size());

//             if ((int)moves.size() >= detachDepth) {
//                 cfg->threadPool->schedule([=] {
//                     generateLevelSingleThread(targetDepth, moves.size(), moves, ss, validMoves);
//                 });
//             } else {
//                 generateLevelMultiThread(targetDepth, detachDepth, moves, ss, validMoves);
//             }
//         }
//         moves.back()++;
//     }
// }

template <int width>
void PruningStates<width>::generateLevel(int lvl) {
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

    int detachDepth = 0;
    int detachWidth = 1;

    while (detachWidth < cfg->threadPool->getNumThreads()) {
        detachWidth *= numChoices;
        detachDepth++;
        if (detachDepth >= targetDepth) { break; }
    }

    detachDepth += 3;
    detachWidth *= numChoices * numChoices * numChoices;

    if (detachDepth >= targetDepth || cfg->threadPool->getNumThreads() == 1) {
        generateLevelSingleThread(targetDepth, moves.size(), moves, ss, validMoves);
        return;
    }

    cfg->log << "targetThreads: " << cfg->threadPool->getNumThreads() << endl;
    cfg->log << "detachWidth: " << detachWidth << endl;
    cfg->log << "detachDepth: " << detachDepth << endl;


    generateLevelMultiThread(targetDepth, detachDepth, moves, ss, validMoves);


    cfg->threadPool->wait();
}

template <int width>
bool PruningStates<width>::canDiscardMoves(int movesAvailable, const vector<int>& moves) {
    return redundancyTableInverse.contains(moves);
}

template <int width>
inline bool PruningStates<width>::checkVisited(State su) {
    auto s = puzzle.compressState(su);
    return visited.count(s);
}

template <int width>
inline bool PruningStates<width>::checkVisited(State su, int numMoves) {
    // return false;
    // auto s = preHashTransformation(state);
    auto s = puzzle.compressState(su);
    if (visited.count(s)) {
        if (numMoves < visited[s]) {
            visited[s] = numMoves;
            return false;
        }
        return true;
    } else {
        visited[s] = numMoves;
        return false;
    }
}
