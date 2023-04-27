#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <regex>
#include <set>
#include <sstream>
#include <stickersolve/solver/puzzle.h>
#include <string>
#include <tuple>
#include <vector>

using namespace std::chrono;
using namespace std;

inline bool Puzzle::checkIfAllMovesHaveInverses() {
    for (auto move : validMoves) {
        bool hasInverse = false;

        for (auto potentialInverse : validMoves) {
            if ((move + potentialInverse).isNOP()) {
                hasInverse = true;
                break;
            }
        }

        if (!hasInverse) { return false; }
    }

    return true;
}


inline string Puzzle::toString() {
    string result = "[";
    for (size_t i = 0; i < state.size(); i++) {
        if (i != 0) { result += ", "; }
        result += to_string(state[i]);
    }
    return result + "]";
}

inline Puzzle::Puzzle() {}
inline Puzzle::Puzzle(State s) {
    solvedState = s;
    state = s;
}

inline void Puzzle::addMove(string name, State move) {
    validMoves.push_back(move);
    moveNames.push_back(name);
}

inline int Puzzle::getMoveID(string name) {
    for (size_t i = 0; i < moveNames.size(); i++) {
        if (moveNames[i] == name) return i;
    }

    return -1;
}

inline std::string Puzzle::getMoveName(int id) { return moveNames[id]; }

inline std::string Puzzle::getMoveName(State mov) {
    for (size_t i = 0; i < validMoves.size(); i++) {
        if (validMoves[i] == mov) return moveNames[i];
    }
    return "";
}


inline State& Puzzle::getMove(int id) { return validMoves[id]; }

inline State& Puzzle::getMove(string name) { return validMoves[getMoveID(name)]; }

inline vector<uint8_t> Puzzle::compressState(const State& s) {
    vector<uint8_t> result;
    for (size_t i = 0; i < s.size(); i++) { result.push_back((uint8_t)(s[i])); }
    return result;
}

inline map<int, int> Puzzle::buildInverseTable() {
    map<int, int> result;
    for (size_t i = 0; i < validMoves.size(); i++) {
        for (size_t j = 0; j < validMoves.size(); j++) {
            if ((validMoves[i] + validMoves[j]).isNOP()) {
                result.insert({i, j});
                break;
            }
        }
    }
    return result;
}

inline uint64_t Puzzle::getChecksum() const {
    const uint64_t goldenRatioConstant = 0x9e3779b97f4a7c15;
    uint64_t result = 0;

    result ^= solvedState.toHash();

    for (size_t i = 0; i < validMoves.size(); i++) {
        result ^= validMoves[i].toHash() * ((goldenRatioConstant << i) | (goldenRatioConstant >> (64 - i)));
    }

    return result;
}

inline void Puzzle::applyMoves(string str) {
    if (state.size() != solvedState.size()) { state = solvedState; }
    std::regex regex{R"([\s]+)"}; // split on space
    std::sregex_token_iterator it{str.begin(), str.end(), regex, -1};
    std::vector<std::string> moves{it, {}};

    for (size_t i = 0; i < moves.size(); i++) { this->state += getMove(moves[i]); }
}

inline void Puzzle::keepOnlyMoves(set<string> keep) {
    vector<string> newMoveNames;
    vector<State> newMoveSates;
    for (size_t i = 0; i < validMoves.size(); i++) {
        if (keep.count(moveNames[i])) {
            newMoveNames.push_back(moveNames[i]);
            newMoveSates.push_back(validMoves[i]);
        }
    }
    moveNames = newMoveNames;
    validMoves = newMoveSates;
}

inline void Puzzle::deleteMoves(set<string> discard) {
    vector<string> newMoveNames;
    vector<State> newMoveSates;
    for (size_t i = 0; i < validMoves.size(); i++) {
        if (!discard.count(moveNames[i])) {
            newMoveNames.push_back(moveNames[i]);
            newMoveSates.push_back(validMoves[i]);
        }
    }
    moveNames = newMoveNames;
    validMoves = newMoveSates;
}

inline void Puzzle::deleteMove(string m) { deleteMoves({m}); }

inline void Puzzle::keepOnlyMoves(string allowedMoves) {
    std::regex regex{R"([\s]+)"}; // split on space
    std::sregex_token_iterator it{allowedMoves.begin(), allowedMoves.end(), regex, -1};
    std::set<std::string> movesToInit{it, {}};
    keepOnlyMoves(movesToInit);
}

inline void Puzzle::deleteMoves(string movesDenied) {
    std::regex regex{R"([\s]+)"}; // split on space
    std::sregex_token_iterator it{movesDenied.begin(), movesDenied.end(), regex, -1};
    std::set<std::string> movesToBlock{it, {}};
    deleteMoves(movesToBlock);
}

inline Puzzle& Puzzle::operator=(initializer_list<int> il) {
    state = il;
    return *this;
}

inline int& Puzzle::operator[](int i) { return state[i]; }

inline Puzzle::operator State() { return state; }

inline std::set<std::string> Puzzle::getMoves() {
    std::set<std::string> r;
    r.insert(moveNames.begin(), moveNames.end());
    return r;
}

inline Puzzle Puzzle::makeUniqueStickers() {
    Puzzle res = *this;
    for (size_t i = 0; i < solvedState.size(); i++) { res.solvedState[i] = i; }
    res.state = res.solvedState;
    return res;
}

inline std::vector<std::vector<int>> Puzzle::getStickerSets() {
    std::vector<std::vector<int>> result;
    State before = makeUniqueStickers();
    for (auto m : validMoves) {
        State after = before + m;
        std::vector<int> set1;
        std::vector<int> set2;
        for (size_t i = 0; i < before.size(); i++) {
            if (before[i] == after[i]) {
                set1.push_back(i);
            } else {
                set2.push_back(i);
            }
        }
        std::vector<std::vector<int>> newResult;
        for (auto s : result) {
            std::vector<int> inter1;
            std::vector<int> inter2;

            std::set_intersection(set1.begin(), set1.end(), s.begin(), s.end(), std::inserter(inter1, inter1.end()));
            std::set_intersection(set2.begin(), set2.end(), s.begin(), s.end(), std::inserter(inter2, inter2.end()));

            if (!inter1.empty()) newResult.push_back(inter1);
            if (!inter2.empty()) newResult.push_back(inter2);
        }
        if (result.empty()) {
            if (!set1.empty()) newResult.push_back(set1);
            if (!set2.empty()) newResult.push_back(set2);
        }
        result = newResult;
    }
    return result;
}

inline std::vector<int> rotateVector(std::vector<int> in, size_t shift) {
    std::vector<int> out;
    size_t idx = shift;
    for (size_t i = 0; i < in.size(); i++) { out.push_back(in[(idx + i) % in.size()]); }
    return out;
}

/// @brief get unique sticker grops by piece type
/// @return
inline std::vector<std::vector<std::vector<int>>> Puzzle::getStickerGroups() {
    auto joinTwoStates = [](State one, State two) {
        State result;
        for (size_t i = 0; i < one.size(); i++) {
            int val = one[i];
            if (val == -1) val = two[i];
            result.push_back(val);
        }
        return result;
    };

    std::vector<std::vector<std::vector<int>>> result;

    std::vector<std::vector<int>> pieceSets = getStickerSets();
    State ref;
    for (size_t i = 0; i < solvedState.size(); i++) { ref.push_back(-1); }
    int counter = 0;
    while (!pieceSets.empty()) {
        auto piece = pieceSets[0];
        for (auto sticker : piece) { ref[sticker] = counter++; }
        //cout << ref.toString() << endl;
        std::vector<State> tryMoves = validMoves;
        while (!tryMoves.empty()) {
            std::vector<State> skipedMoved;
            int movesCauseingChanges = 0;
            for (auto& move : tryMoves) {
                if (ref == ref + move) {
                    skipedMoved.push_back(move);
                    continue;
                }
                movesCauseingChanges++;
                //cout << this->getMoveName(move);
                ref = joinTwoStates(ref, ref + move);
                //cout << ref.toString() << endl;
            }
            tryMoves = skipedMoved;
            if (movesCauseingChanges == 0) { tryMoves = {}; }
        }
        std::vector<std::vector<int>> leftoverPieces;
        result.emplace_back();
        for (auto p : pieceSets) {
            bool isLeftover = true;
            for (auto s : p) {
                if (ref[s] != -1) isLeftover = false;
            }
            if (isLeftover) {
                leftoverPieces.push_back(p);
            } else {
                result.back().push_back(p);
            }
        }
        pieceSets = leftoverPieces;
    }

    for (auto& grp : result) {
        std::sort(grp.begin(), grp.end(), [](const std::vector<int>& a, const std::vector<int>& b) {
            int aMin = *std::min_element(a.begin(), a.end());
            int bMin = *std::min_element(b.begin(), b.end());
            return aMin < bMin;
        });
        for (auto& piece : grp) {
            std::map<int, int> sorted;
            for (auto& sticker : piece) { sorted[ref[sticker]] = sticker; }
            piece.clear();
            for (auto& s : sorted) { piece.push_back(s.second); }
        }
    }
    std::sort(
        result.begin(),
        result.end(),
        [](const std::vector<std::vector<int>>& a, const std::vector<std::vector<int>>& b) {
            int aMin = *std::min_element(a[0].begin(), a[0].end());
            int bMin = *std::min_element(b[0].begin(), b[0].end());
            return aMin < bMin;
        }
    );

    // TODO: delete this, this makes it easy to debug for now
    // for (auto& grp : result) {
    //     for (auto& piece : grp) {
    //         size_t maxWeightIdx = 0;
    //         int maxWeight = -1;
    //         for (size_t i = 0; i < piece.size(); i++) {
    //             int newWeight = puzzleOrientationPriority[piece[i]];
    //             if (newWeight > maxWeight) {
    //                 maxWeight = newWeight;
    //                 maxWeightIdx = i;
    //             }
    //         }
    //         piece = rotateVector(piece, maxWeightIdx);
    //     }
    // }

    return result;
}

// given a piece type (corner, edge ...) and a list of colors, return the id of the piece
inline std::map<std::pair<int, std::vector<int>>, std::vector<int>> Puzzle::getStickerMap() {
    std::vector<std::vector<std::vector<int>>> pieceTypes = getStickerGroups();

    std::map<std::pair<int, std::vector<int>>, std::vector<int>> result;
    for (size_t pieceType = 0; pieceType < pieceTypes.size(); pieceType++) {
        auto& pieces = pieceTypes[pieceType];
        for (auto& piece : pieces) {
            std::vector<int> colors;
            for (auto& sticker : piece) { colors.push_back(solvedState[sticker]); }
            if (!result.count({pieceType, colors})) {
                vector<int> ids;
                for (size_t pn = 0; pn < piece.size(); pn++) ids.push_back(piece[pn]);
                result[{pieceType, colors}] = ids;
            }
        }
    }

    return result;
}

inline State Puzzle::getPieceState(State s) {
    State result = s;
    for (size_t i = 0; i < s.size(); i++) { result[i] = -1; }
    std::map<std::pair<int, std::vector<int>>, std::vector<int>> m = getStickerMap();
    std::vector<std::vector<std::vector<int>>> pieceTypes = getStickerGroups();

    for (size_t pieceType = 0; pieceType < pieceTypes.size(); pieceType++) {
        auto& pieces = pieceTypes[pieceType];
        for (auto piece : pieces) {
            std::vector<int> colors;
            for (auto& sticker : piece) { colors.push_back(s[sticker]); }
            // colors = standardizeColorCycle(colors);
            for (size_t i = 0; i < piece.size(); i++) {
                colors = rotateVector(colors, 1);
                piece = rotateVector(piece, 1);
                if (m.count({pieceType, colors})) {
                    for (size_t pn = 0; pn < piece.size(); pn++) result[piece[pn]] = m[{pieceType, colors}][pn];
                    break;
                }
            }
            if (!m.count({pieceType, colors}))
                throw std::runtime_error("mapping to unique stickers failed, could not find piece in color map, puzzle "
                                         "state doesnt have the same sticker set as the solved state.");
        }
    }
    return result;
}

inline Puzzle Puzzle::getPiecePuzzle() {
    Puzzle result = *this;
    result.solvedState = getPieceState(solvedState);
    result.state = getPieceState(state);
    return result;
}
