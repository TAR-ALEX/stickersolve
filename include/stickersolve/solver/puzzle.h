#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <stickersolve/solver/puzzleState.h>

using namespace std;

class Puzzle {
protected:
    // WARNING: This works only when number of stickers is less than 256, (dont use for redundancyTable in the future)
    vector<uint8_t> compressState(State& s);

    vector<State> validMoves;
    vector<string> moveNames;

    State puzzleOrientationPriority;

    std::vector<int> prioritizeColors(std::vector<int> in);
public:
    std::vector<std::vector<int>> getStickerSets();
    std::vector<std::vector<std::vector<int>>> getStickerGroups();

    std::map<std::pair<int, std::vector<int>>, std::vector<int>> getStickerMap();

    State state;
    State solvedState;

    Puzzle makeUniqueStickers();

    vector<int> invertMoves(vector<int>);
    uint64_t getChecksum() const;
    bool checkIfAllMovesHaveInverses();
    map<int, int> buildInverseTable();

    Puzzle(const Puzzle&) = default;
    Puzzle(Puzzle&&) = default;
    Puzzle();
    Puzzle(State s);
    Puzzle& operator=(const Puzzle& x) = default;
    Puzzle& operator=(Puzzle&& x) = default;
    Puzzle& operator=(initializer_list<int> il);
    int& operator[](int i);

    void keepOnlyMoves(string);
    void deleteMoves(string);
    std::set<string> getMoves();

    void keepOnlyMoves(set<string>);
    void deleteMoves(set<string>);

    void addMove(string name, State move);
    void deleteMove(string name);
    State& getMove(string name);
    State& getMove(int name);
    int getMoveID(string name);
    std::string getMoveName(int id);
    std::string getMoveName(State mov);

    Puzzle getPermutationPuzzle(); // delete
    Puzzle getOrientationPuzzle(); // delete
    State getPieceState(State s);
    Puzzle getPiecePuzzle();

    void applyMoves(string moves);

    string toString();

    operator State();

    friend class Solver;
    friend class RedundancyTable;
    friend class PruningStates;
};

void printMoves(vector<string>& moveNames, vector<int> moves);

