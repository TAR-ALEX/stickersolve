#pragma once

#include <regex>
#include <stickersolve.h>
#include <stickersolve/puzzles/Puzzle3x3.h>

using namespace std;

namespace PruningFor3x3 {
    struct MaskPermutaion : public PruningStates<1> {
        Puzzle3x3 sym;

        State recolorMask = {
            0,  1,  2,  3,  4,  5,  6,  7,  8,  //
            9,  10, 11, 12, 13, 14, 15, 16, 17, //
            18, 19, 20, 21, 22, 23, 24, 25, 26, //
            27, 28, 29, 30, 31, 32, 33, 34, 35, //
            36, 37, 38, 39, 40, 41, 42, 43, 44, //
            45, 46, 47, 48, 49, 50, 51, 52, 53  //
        };

        virtual State preInsertTransformation(State s) { return sym.getUniqueSymetric(s); }
        int cannotBeSolvedInLimit(int movesAvailable, const State& state) {
            // return PruningStates::cannotBeSolvedInLimit(movesAvailable, sym.getUniqueSymetric(state.recolor(recolorMask)));
            return PruningStates::cannotBeSolvedInLimit(movesAvailable, sym.getUniqueSymetric(state));
            // return PruningStates::cannotBeSolvedInLimit(movesAvailable, state.recolor(recolorMask));
        }
        void init(std::string allowed = "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'", std::string prefix = "") {
            sym = Puzzle3x3(allowed).getPiecePuzzle();
            sym.solvedState = recolorMask;
            sym.state = recolorMask;
            this->puzzle = sym;
            sym.generateSymetryTable();
            this->depth = 10;
            this->hashSize = 33;
            this->cfg = cfg;
            this->path = prefix + "MaskPermutation.table";
        }
        MaskPermutaion() { init(); }
    };
    struct Mask3Color : public PruningStates<0> {
        Puzzle3x3 sym;
        State recolorMask = {
            0,  1,  2,  3,  4, 3,  2,  1,  0,  //
            27, 28, 29, 12, 4, 12, 29, 28, 27, //
            36, 37, 20, 41, 4, 41, 20, 37, 36, //
            27, 28, 29, 12, 4, 12, 29, 28, 27, //
            36, 37, 20, 41, 4, 41, 20, 37, 36, //
            0,  1,  2,  3,  4, 3,  2,  1,  0,  //
        };
        virtual State preInsertTransformation(State s) { return sym.getUniqueSymetric(s); }
        int cannotBeSolvedInLimit(int movesAvailable, const State& state) {
            return PruningStates::cannotBeSolvedInLimit(
                movesAvailable, sym.getUniqueSymetric(state.recolor(recolorMask))
            );
        }
        void init(std::string allowed = "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'", std::string prefix = "") {
            sym = Puzzle3x3(allowed).getPiecePuzzle();
            sym.solvedState = recolorMask;
            sym.state = recolorMask;
            this->puzzle = sym;
            sym.generateSymetryTable();
            this->depth = 11;
            this->hashSize = 35;
            this->cfg = cfg;
            this->path = prefix + "Mask3Color.table";
        }
        Mask3Color() { init(); }
    };
}; // namespace PruningFor3x3

template <bool HAS_SLICES = false> // 0 is htm, 1 is stm, 2 is qtm
class Solver3x3Restricted : public Solver {
private:
    string allowedMoves = "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'";

public:
    RedundancyTable redundancyTable;
    RedundancyTable redundancyTableInverse;

    // PruningStates<2> pruningTableClassic;
    PruningFor3x3::Mask3Color pruning3Color;
    PruningFor3x3::MaskPermutaion testTable;

    Solver3x3Restricted() : Solver() {
        if (HAS_SLICES)
            allowedMoves = "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B' RL RL2 RL' UD UD2 UD' FB FB2 FB'";
        else
            allowedMoves = "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'";
        puzzle = Puzzle3x3().getPiecePuzzle();
        puzzle.keepOnlyMoves(allowedMoves);

        // pruningTableClassic.puzzle = puzzle;

        // pruningTableClassic.depth = 7; // 8 HTM 10 rfu
        // pruningTableClassic.hashSize = 27;
        // pruningTableClassic.path = "";
        // pruningTableClassic.cfg = cfg;
    }

    int progress1 = 0;
    int progress2 = 0;

    void init() {
        redundancyTable.depth = 3; //3
        redundancyTable.puzzle = puzzle;
        redundancyTable.cfg = cfg;

        pruning3Color.cfg = cfg;
        testTable.cfg = cfg;

        pruning3Color.progressCallback = [&](int p) {
            progress1 = p;
            tableProgressCallback((progress1 + progress2) / 2);
        };
        testTable.progressCallback = [&](int p) {
            progress2 = p;
            tableProgressCallback((progress1 + progress2) / 2);
        };

        pruning3Color.init(allowedMoves, HAS_SLICES ? "1" : "0");
        testTable.init(allowedMoves, HAS_SLICES ? "1" : "0");

        if constexpr (HAS_SLICES) {
            pruning3Color.depth -= 2;
            testTable.depth -= 2;
        }

        redundancyTable.load();
        // pruningTableClassic.load();
        pruning3Color.load();
        // pruning3ColorB.load();
        // pruningRing.load();
        testTable.load();
    }

    std::string printTableStats() {
        stringstream ss;
        auto printStat = [&](std::string name, std::string stat) {
            ss << name;
            ss << "\n--------------------\n";
            ss << stat;
            ss << "\n--------------------\n";
        };
        printStat("redundancyTable.getStats()", redundancyTable.getStats());
        // printStat("pruningTableClassic.getStats()", pruningTableClassic.getStats());
        printStat("pruning3Color.getStats()", pruning3Color.getStats());
        printStat("testTable.getStats()", testTable.getStats());

        return ss.str();
    }
    Puzzle3x3 sym = Puzzle3x3("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'").getPiecePuzzle();

    virtual State preInsertTransformation(State s) { return sym.getUniqueSymetric(s); }

    virtual State preSolveTransform(State s1) {
        // do more testing here compare stock piece state to new one
        State s2 = Puzzle3x3().getPieceState(s1);
        cout << s2.toString() << endl;
        return s2;
    }

    virtual bool canDiscardMoves(int movesAvailable, const vector<int>& moves) {
        return redundancyTable.contains(moves);
    }

    virtual bool canDiscardPosition(int movesAvailable, const State& stateReal) {
        if (movesAvailable <= 2) return false;
        if constexpr (HAS_SLICES == 1) {
            State so = sym.getStandardOrientation(stateReal);
            if (pruning3Color.cannotBeSolvedInLimit(movesAvailable, so) == 1) return true;
            if (testTable.cannotBeSolvedInLimit(movesAvailable, so) == 1) return true;
            return false;
        } else if constexpr (HAS_SLICES == 0) {
            if (pruning3Color.cannotBeSolvedInLimit(movesAvailable, stateReal) == 1) return true;
            if (testTable.cannotBeSolvedInLimit(movesAvailable, stateReal) == 1) return true;
            return false;
        }
    }
};

class Solver3x3 : public SolverAutoSelector {
    Solver3x3Restricted<0> HTM;
    Solver3x3Restricted<1> STM;
    virtual void selectSolver(Puzzle initial) {
        HTM.cfg = cfg;
        STM.cfg = cfg;

        if (initial.getMoves().count("M")) selected = &STM;
        else
            selected = &HTM;
    }

public:
    // Solver3x3(std::string s) : Solver3x3() {}
    Solver3x3() {}
};
