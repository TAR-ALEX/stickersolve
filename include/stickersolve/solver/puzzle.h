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
private:
    // WARNING: This works only when number of stickers is less than 256, (dont use for redundancyTable in the future)
    vector<uint8_t> compressState(State& s);

    State state;
    State solvedState;
    vector<State> validMoves;
    vector<string> moveNames;

public:
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

    void keepOnlyMoves(set<string>);
    void deleteMoves(set<string>);

    void addMove(string name, State move);
    void deleteMove(string name);
    State& getMove(string name);
    State& getMove(int name);
    int getMoveID(string name);


    void applyMoves(string moves);

    string toString();

    operator State();

    friend class Solver;
    friend class RedundancyTable;
    friend class PruningStates;
};

void printMoves(vector<string>& moveNames, vector<int> moves);
