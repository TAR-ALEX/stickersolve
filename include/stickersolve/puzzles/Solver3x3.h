#pragma once

#include <regex>
#include <stickersolve.h>
#include <stickersolve/puzzles/Puzzle3x3.h>

using namespace std;

namespace PruningFor3x3 {
    struct MaskPermutaion : public PruningStates<1> {
        Puzzle3x3 sym;
        // State recolorMask = {
        //     0,   1,  2,  3,  4,  5,  6,  7,   8, // regular permutation
        //     6,   7,  8, 12,  4,  14, 45, 46, 47, //
        //     8,   5,  2, 14,  4,  30, 47, 50, 53, //
        //     2,   1,  0, 30,  4,  32, 53, 52, 51, //
        //     0,   3,  6, 32,  4,  12, 51, 48, 45, //
        //     45, 46, 47, 48,  4,  50, 51, 52, 53  //
        // };
        State recolorMask = {
            0,  1,  2,  3,  4,  5,  6,  7,  8,  //
            9,  10, 11, 12, 13, 14, 15, 16, 17, //
            18, 19, 20, 21, 22, 23, 24, 25, 26, //
            27, 28, 29, 30, 31, 32, 33, 34, 35, //
            36, 37, 38, 39, 40, 41, 42, 43, 44, //
            45, 46, 47, 48, 49, 50, 51, 52, 53  //
        };

        virtual State preInsertTransformation(State s) {
            return sym.getUniqueSymetric(s);
        }
        int cannotBeSolvedInLimit(int movesAvailable, const State& state) {   
            // return PruningStates::cannotBeSolvedInLimit(movesAvailable, sym.getUniqueSymetric(state.recolor(recolorMask)));
            return PruningStates::cannotBeSolvedInLimit(movesAvailable, sym.getUniqueSymetric(state));
            // return PruningStates::cannotBeSolvedInLimit(movesAvailable, state.recolor(recolorMask));
        }
        MaskPermutaion(estd::joint_ptr<SolverConfig> cfg = nullptr) {
            sym = Puzzle3x3("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'").getPiecePuzzle();
            sym.solvedState = recolorMask;
            sym.state = recolorMask;
            this->puzzle = sym;
            sym.generateSymetryTable();
            this->depth = 10;
            this->hashSize = 33;
            this->cfg = cfg;
            this->path = "MaskPermutation.table";
        }
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
        virtual State preInsertTransformation(State s) {
            return sym.getUniqueSymetric(s);
        }
        int cannotBeSolvedInLimit(int movesAvailable, const State& state) {   
            return PruningStates::cannotBeSolvedInLimit(movesAvailable, sym.getUniqueSymetric(state.recolor(recolorMask)));
        }
        Mask3Color(estd::joint_ptr<SolverConfig> cfg = nullptr) {
            sym = Puzzle3x3("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'").getPiecePuzzle();
            sym.solvedState = recolorMask;
            sym.state = recolorMask;
            this->puzzle = sym;
            sym.generateSymetryTable();
            this->depth = 11;
            this->hashSize = 35;
            this->cfg = cfg;
            this->path = "Mask3Color.table";
        }
    };
    struct MaskRing : public PruningStates<1> {
        Puzzle3x3 sym;
        State recolorMask = {
            0,  1,  2,  3,  4,  3,  6,  1,  8,  //
            9,  10, 11, 12, 13, 12, 15, 10, 17, //
            18, 19, 20, 21, 22, 21, 24, 19, 26, //
            27, 10, 29, 12, 31, 12, 33, 10, 35, //
            36, 19, 38, 21, 40, 21, 42, 19, 44, //
            45, 1,  47, 3,  49, 3,  51, 1,  53  //
        };
        virtual State preInsertTransformation(State s) {
            return sym.getUniqueSymetric(s);
        }
        int cannotBeSolvedInLimit(int movesAvailable, const State& state) {
            static auto rotX = Puzzle3x3().getMove("x");
            static auto rotXp = Puzzle3x3().getMove("x'");
            static auto rotZ = Puzzle3x3().getMove("z");
            static auto rotZp = Puzzle3x3().getMove("z'");
            State tmp = state;
            switch(PruningStates::cannotBeSolvedInLimit(movesAvailable, sym.getUniqueSymetric(tmp.recolor(recolorMask)))){
                case 1: return 1;
                case 0: return 0;
            }
            tmp = (state + rotX).recolor(rotXp);
            if (PruningStates::cannotBeSolvedInLimit(movesAvailable, sym.getUniqueSymetric(tmp.recolor(recolorMask))) == 1) return 1;
            tmp = (state + rotZ).recolor(rotZp);
            if (PruningStates::cannotBeSolvedInLimit(movesAvailable, sym.getUniqueSymetric(tmp.recolor(recolorMask))) == 1) return 1;
            return -1;
        }
        MaskRing(estd::joint_ptr<SolverConfig> cfg = nullptr) {
            sym = Puzzle3x3{
                "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'",
            };
            sym.solvedState = recolorMask;
            sym.state = recolorMask;
            this->puzzle = sym;
            sym.generateSymetryTable();
            this->depth = 10;
            this->hashSize = 31;
            this->cfg = cfg;
            this->path = "MaskRing.table";
        }
    };
}; // namespace PruningFor3x3

class Solver3x3 : public Solver {
public:
    RedundancyTable redundancyTable;
    RedundancyTable redundancyTableInverse;

    PruningStates<2> pruningTableClassic;
    PruningFor3x3::Mask3Color pruning3Color;
    PruningFor3x3::MaskRing pruningRing;
    PruningFor3x3::MaskPermutaion testTable;

    // Puzzle3x3 origPuzzle;

    Solver3x3() : Solver3x3("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'"){};
    Solver3x3(string allowedMoves) : Solver() {
        // origPuzzle = Puzzle3x3(allowedMoves);
        puzzle = Puzzle3x3().getPiecePuzzle();
        puzzle.keepOnlyMoves(allowedMoves);
        // for(auto m: puzzle.getMoves()) cout << m << " "; cout << endl;
        cout << puzzle.toString() << endl;

        redundancyTable.depth = 3;//3
        redundancyTable.puzzle = puzzle;
        redundancyTable.cfg = cfg;

        pruning3Color.cfg = cfg;
        pruningRing.cfg = cfg;
        testTable.cfg = cfg;

        pruningTableClassic.puzzle = puzzle;

        pruningTableClassic.depth = 7; // 8 HTM 10 rfu
        pruningTableClassic.hashSize = 27;
        pruningTableClassic.path = "";
        pruningTableClassic.cfg = cfg;
    }

    void init() {
        int progress1 = 0;
        int progress2 = 0;

        pruning3Color.progressCallback = [&](int p){
            progress1 = p;
            tableProgressCallback((progress1 + progress2)/2);
        };
        testTable.progressCallback = [&](int p){
            progress2 = p;
            tableProgressCallback((progress1 + progress2)/2);
        };
        

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

    virtual State preInsertTransformation(State s) {
        return sym.getUniqueSymetric(s);
    }

    virtual State preSolveTransform(State s1) {
        State s2 = Puzzle3x3().getPieceState(s1);
        cout << s2.toString() << endl;
        return s2;
    }

    virtual bool canDiscardMoves(int movesAvailable, const vector<int>& moves) {
        return redundancyTable.contains(moves);
    }

    virtual bool canDiscardPosition(int movesAvailable, const State& stateReal) {
        if (movesAvailable <= 2) return false;

        // switch (pruningTableClassic.cannotBeSolvedInLimit(movesAvailable, stateReal)) {
        //     case 1: return true;
        //     case 0:
                if (pruning3Color.cannotBeSolvedInLimit(movesAvailable, stateReal) == 1) return true; // 10.7 [4.29]
                if (testTable.cannotBeSolvedInLimit(movesAvailable, stateReal) == 1) return true;     // 10.35 [7.41]
        // }

        return false;
    }
};
