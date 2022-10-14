#pragma once

#include <regex>
#include <stickersolve/puzzles/Puzzle3x3.h>
#include <stickersolve/solver/PuzzleSolver.h>

using namespace std;

namespace PruningFor3x3 {
    struct Mask3Color : public PruningStates<1> {
        State recolorMask = {
            0,  1,  2,  3,  -1,  3,  2,  1,  0,  //
            -1, -1, -1, 12, -1, 12, -1, -1, -1, //
            -1, -1, -1, -1, -1, -1, -1, -1, -1, //
            -1, -1, -1, 12, -1, 12, -1, -1, -1, //
            -1, -1, -1, -1, -1, -1, -1, -1, -1, //
            0,  1,  2,  3,  -1, 3,  2,  1,  0,  //
        };
        // virtual State preInsertTransformation(State s){
        //     static auto pzl = Puzzle3x3();
        //     // return s.recolor(recolorMask);
        //     return pzl.getUniqueSymetric(s).recolor(recolorMask);
        // }
        // virtual State preLookupTransformation(State s){
        //     static auto pzl = Puzzle3x3();
        //     // return s.recolor(recolorMask);
        //     return pzl.getUniqueSymetric(s).recolor(recolorMask);
        // }
        bool cannotBeSolvedInLimit(int movesAvailable, const State& state) {
            return PruningStates::cannotBeSolvedInLimit(movesAvailable, state.recolor(recolorMask));
        }
        Mask3Color(estd::joint_ptr<SolverConfig> cfg = nullptr) {
            this->puzzle = Puzzle3x3{
                "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'",
            }.getPiecePuzzle();
            this->puzzle.solvedState = recolorMask;
            this->puzzle.state = this->puzzle.solvedState;
            this->depth = 9;
            this->hashSize = 30;
            this->cfg = cfg;
            this->path = "Mask3Color-nosym.table";
        }
    };
     struct Mask3ColorB : public PruningStates<0> {
        State recolorMask = {
            0,  1,  2,  3,  4,  3,  2,  1,  0,  //
            -1, -1, -1, 12, 13, 12, -1, -1, -1, //
            -1, -1, -1, -1, 22, -1, -1, -1, -1, //
            -1, -1, -1, 12, -1, 12, -1, -1, -1, //
            -1, -1, -1, -1, -1, -1, -1, -1, -1, //
            0,  1,  2,  3,  -1, 3,  2,  1,  0,  //
        };
        bool cannotBeSolvedInLimit(int movesAvailable, const State& state) {
            return PruningStates::cannotBeSolvedInLimit(movesAvailable, state.recolor(recolorMask));
        }
        Mask3ColorB(estd::joint_ptr<SolverConfig> cfg = nullptr) {
            this->puzzle = Puzzle3x3{
                "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'",
            };
            this->puzzle.solvedState = recolorMask;
            this->puzzle.state = this->puzzle.solvedState;
            this->depth = 9;
            this->hashSize = 34;
            this->cfg = cfg;
            this->path = "Mask3ColorBin.table";
        }
    };
    struct MaskRing : public PruningStates<1> {
        State recolorMask = {
            1, 1, 1, 1, 0, 1, 1, 1, 1, //
            0, 0, 0, 2, 0, 2, 0, 0, 0, //
            0, 0, 0, 0, 0, 0, 0, 0, 0, //
            0, 0, 0, 2, 0, 2, 0, 0, 0, //
            0, 0, 0, 0, 0, 0, 0, 0, 0, //
            1, 1, 1, 1, 0, 1, 1, 1, 1, //
        };
        bool cannotBeSolvedInLimit(int movesAvailable, const State& state) {
            static auto rotX = Puzzle3x3().getMove("x");
            static auto rotXp = Puzzle3x3().getMove("x'");
            static auto rotZ = Puzzle3x3().getMove("z");
            static auto rotZp = Puzzle3x3().getMove("z'");
            State tmp = state;

            if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;
            tmp = (state + rotX).recolor(rotXp);
            if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;
            tmp = (state + rotZ).recolor(rotZp);
            if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;
            return false;
        }
        MaskRing(estd::joint_ptr<SolverConfig> cfg = nullptr) {
            this->puzzle = Puzzle3x3{
                "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'",
            };
            this->puzzle.solvedState = recolorMask;
            this->puzzle.state = this->puzzle.solvedState;
            this->depth = 10;    //10
            this->hashSize = 32; //this->hashSize = 31;
            this->cfg = cfg;
            this->path = "MaskRingB.table";
        }
    };
    struct Mask3ColorCornersEO : public PruningStates<1> {
        // State recolorMask = {
        //     1, 2, 3, 2, 5, 2, 3, 2, 1, //
        //     0, 0, 0, 8, 0, 8, 0, 0, 0, //
        //     0, 0, 0, 0, 0, 0, 0, 0, 0, //
        //     0, 0, 0, 8, 0, 8, 0, 0, 0, //
        //     0, 0, 0, 0, 0, 0, 0, 0, 0, //
        //     1, 2, 3, 2, 5, 2, 3, 2, 1, //
        // };
        State recolorMask = {
            1, 2, 3, 2, 5, 2, 3, 2, 1, //
            0, 0, 0, 8, 0, 8, 0, 0, 0, //
            0, 0, 0, 8, 0, 8, 0, 0, 0, //
            0, 0, 0, 8, 0, 8, 0, 0, 0, //
            0, 0, 0, 8, 0, 8, 0, 0, 0, //
            1, 2, 3, 2, 5, 2, 3, 2, 1, //
        };
        bool cannotBeSolvedInLimit(int movesAvailable, const State& state) {
            static auto rotX = Puzzle3x3().getMove("x");
            static auto rotXp = Puzzle3x3().getMove("x'");
            static auto rotZ = Puzzle3x3().getMove("z");
            static auto rotZp = Puzzle3x3().getMove("z'");
            static auto rotY = Puzzle3x3().getMove("y");
            static auto rotYp = Puzzle3x3().getMove("y'");
            State tmp = state;

            if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;
            tmp = (tmp + rotX).recolor(rotXp);
            if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;
            tmp = (tmp + rotX).recolor(rotXp);
            if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;

            // tmp = (tmp + rotZ).recolor(rotZp);
            // if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;
            // tmp = (tmp + rotX).recolor(rotXp);
            // if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;
            // tmp = (tmp + rotX).recolor(rotXp);
            // if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;

            // tmp = (tmp + rotX).recolor(rotXp);
            // if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;
            // tmp = (tmp + rotX).recolor(rotXp);
            // if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;
            // tmp = (tmp + rotX).recolor(rotXp);
            // if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;


            // tmp = (state + rotZ).recolor(rotZp);
            // if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;
            // tmp = (tmp + rotX).recolor(rotXp);
            // if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;
            // tmp = (tmp + rotX).recolor(rotXp);
            // if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;
            // tmp = (tmp + rotX).recolor(rotXp);
            // if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;

            // tmp = (state + rotY).recolor(rotYp);
            // if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;
            // tmp = (tmp + rotX).recolor(rotXp);
            // if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;
            // tmp = (tmp + rotX).recolor(rotXp);
            // if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;
            // tmp = (tmp + rotX).recolor(rotXp);
            // if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;


            return false;
        }
        Mask3ColorCornersEO(estd::joint_ptr<SolverConfig> cfg = nullptr) {
            this->puzzle = Puzzle3x3{
                "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'",
            };
            this->puzzle.solvedState = recolorMask;
            this->puzzle.state = this->puzzle.solvedState;
            this->depth = 9;
            this->hashSize = 32;
            this->cfg = cfg;
            this->path = "Mask3ColorCornersEO.table";
        }
    };
    struct Mask3ColorP : public PruningStates<2> {
        State recolorMask = {
            1, 2, 3, 4, 0, 4, 3, 2, 1, //
            3, 2, 1, 5, 0, 5, 1, 2, 3, //
            1, 4, 3, 5, 0, 5, 3, 4, 1, //
            3, 2, 1, 5, 0, 5, 1, 2, 3, //
            1, 4, 3, 5, 0, 5, 3, 4, 1, //
            1, 2, 3, 4, 0, 4, 3, 2, 1, //
        };
        bool cannotBeSolvedInLimit(int movesAvailable, const State& state) {
            static auto rotX = Puzzle3x3().getMove("x");
            static auto rotXp = Puzzle3x3().getMove("x'");
            static auto rotZ = Puzzle3x3().getMove("z");
            static auto rotZp = Puzzle3x3().getMove("z'");
            State tmp = state;

            if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;
            tmp = (state + rotX).recolor(rotXp);
            if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;
            tmp = (state + rotZ).recolor(rotZp);
            if (PruningStates::cannotBeSolvedInLimit(movesAvailable, tmp.recolor(recolorMask))) return true;
            return false;
        }
        Mask3ColorP(estd::joint_ptr<SolverConfig> cfg = nullptr) {
            this->puzzle = Puzzle3x3{
                "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'",
            };
            this->puzzle.solvedState = recolorMask;
            this->puzzle.state = this->puzzle.solvedState;
            this->depth = 9;
            this->hashSize = 31;
            this->cfg = cfg;
            this->path = "pruning3ColorP.table";
        }
    };
}; // namespace PruningFor3x3

class Solver3x3 : public Solver {
public:
    RedundancyTable redundancyTable;
    RedundancyTable redundancyTableInverse;

    PruningStates<2> pruningTableClassic;
    PruningFor3x3::Mask3Color pruning3Color;
    PruningFor3x3::Mask3ColorB pruning3ColorB;
    PruningFor3x3::MaskRing pruningRing;
    PruningFor3x3::Mask3ColorCornersEO testTable;

    // Puzzle3x3 origPuzzle;

    Solver3x3() : Solver3x3("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'"){};
    Solver3x3(string allowedMoves) : Solver() {
        // origPuzzle = Puzzle3x3(allowedMoves);
        puzzle = Puzzle3x3().getPiecePuzzle();
        puzzle.keepOnlyMoves(allowedMoves);
        // for(auto m: puzzle.getMoves()) cout << m << " "; cout << endl;
        cout << puzzle.toString() << endl;

        redundancyTable.depth = 3;
        redundancyTable.puzzle = puzzle;
        redundancyTable.cfg = cfg;

        pruning3Color.cfg = cfg;
        pruning3ColorB.cfg = cfg;
        pruningRing.cfg = cfg;
        testTable.cfg = cfg;

        pruningTableClassic.puzzle = puzzle;

        pruningTableClassic.depth = 7; // 8 HTM 10 rfu
        pruningTableClassic.hashSize = 27;
        pruningTableClassic.path = "";
        pruningTableClassic.cfg = cfg;
    }

    void init() {
        redundancyTable.load();
        pruningTableClassic.load();
        pruning3Color.load();
        pruning3ColorB.load();
        pruningRing.load();
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
        printStat("pruningTableClassic.getStats()", pruningTableClassic.getStats());
        printStat("pruning3Color.getStats()", pruning3Color.getStats());
        printStat("pruning3ColorB.getStats()", pruning3ColorB.getStats());
        printStat("pruningRing.getStats()", pruningRing.getStats());
        printStat("testTable.getStats()", testTable.getStats());

        return ss.str();
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

        if (pruningTableClassic.cannotBeSolvedInLimit(movesAvailable, stateReal)) return true;
        
        if (movesAvailable >= 8 && pruning3ColorB.cannotBeSolvedInLimit(movesAvailable, stateReal)) return true; // 
        if (pruning3Color.cannotBeSolvedInLimit(movesAvailable, stateReal)) return true; // 10.7 [4.29]
        if (pruningRing.cannotBeSolvedInLimit(movesAvailable, stateReal)) return true; // 9.69

        if (testTable.cannotBeSolvedInLimit(movesAvailable, stateReal)) return true; // 10.35 [7.41]

        return false;
    }
};
