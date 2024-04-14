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

class Puzzle: public estd::clonable {
protected:
    // WARNING: This works only when number of stickers is less than 256, (dont use for redundancyTable in the future)
    vector<uint8_t> compressState(const State& s);

    vector<State> validMoves;
    vector<string> moveNames;

public:
    //returns sets of stickers that always move together
    std::vector<std::vector<int>> getStickerSets();
    //returns sets of stickers that always move together grouped by piece type, (interchangable sets are in one group)
    std::vector<std::vector<std::vector<int>>> getStickerGroups();
    //returns a map that can map each piece type to unique stickers
    std::map<std::pair<int, std::vector<int>>, std::vector<int>> getStickerMap();

    Puzzle makeUniqueStickers();

    State state;
    State solvedState;  

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
    void copyMoves(const Puzzle&);
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

    State getPieceState(State s);
    //returns the puzzle where each sticker has a unique number ascending order based on the solved state. 
    //(if it is impossible to identify a unique sticker, will give the same id, 
    //in this case the puzzle can have more than one solved state, and is not invertable)
    Puzzle getPiecePuzzle();

    void applyMoves(string moves);
    virtual State getUniqueSymetric(const State& s) {return s;}
    virtual State getStandardOrientation(const State& s) {return s;}

    virtual clonable* clone() const{
        return new Puzzle(*this);
    }

    string toString();

    operator State();

    friend class Solver;
    friend class RedundancyTable;
    template <class Puzzle, int width, bool useSym, bool useInverse>
    friend class PruningStates;
};

void printMoves(vector<string>& moveNames, vector<int> moves);
