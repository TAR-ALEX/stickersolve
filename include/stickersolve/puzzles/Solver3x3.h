#pragma once

#include <regex>
#include <stickersolve.h>
#include <stickersolve/puzzles/Puzzle3x3.h>

using namespace std;
#define testInverse true
#define useSym true
namespace PruningFor3x3 {
    struct MaskPermutaion : public PruningStates<Puzzle3x3, 1, useSym, testInverse> {
        State recolorMask = {
            0,  1,  2,  3,  4,  5,  6,  7,  8,  //
            9,  10, 11, 12, 13, 14, 15, 16, 17, //
            18, 19, 20, 21, 22, 23, 24, 25, 26, //
            27, 28, 29, 30, 31, 32, 33, 34, 35, //
            36, 37, 38, 39, 40, 41, 42, 43, 44, //
            45, 46, 47, 48, 49, 50, 51, 52, 53  //
        };

        int cannotBeSolvedInLimit(int movesAvailable, const State& state) {
            if constexpr (!testInverse){
                return PruningStates::cannotBeSolvedInLimit(movesAvailable, puzzle.getUniqueSymetric(state));
            }else{
                return PruningStates::cannotBeSolvedInLimit(movesAvailable, puzzle.getUniqueSymetricInverse(state));
            }
        }
        void init(std::string allowed = "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'", std::string prefix = "") {
            this->puzzle = Puzzle3x3().getPiecePuzzle();
            this->puzzle.solvedState = recolorMask;
            this->puzzle.state = recolorMask;
            this->puzzle.keepOnlyMoves(allowed);
            this->puzzle.generateSymetryTable();
            this->depth = 10;
            this->hashSize = 33;
            this->cfg = cfg;
            this->path = prefix + "MaskPermutation"+(testInverse?"Inv":"")+".table";
        }
        MaskPermutaion() { init(); }
    };
    struct MaskExperiment : public PruningStates<Puzzle3x3, 1, useSym, false> {
        State recolorMask = {
            0,  1,  2,  3,  4,  5,  6,  7,  8,  //
            9,  7,  11, 12, 13, 14, 15, 16, 17, //
            18, 5,  20, 21, 22, 23, 24, 25, 26, //
            27, 1,  29, 30, 31, 32, 33, 34, 35, //
            36, 3,  38, 39, 40, 41, 42, 43, 44, //
            45, 16, 47, 43, 49, 25, 51, 34, 53  //
        };

        int cannotBeSolvedInLimit(int movesAvailable, const State& state) {
            return PruningStates::cannotBeSolvedInLimit(movesAvailable, puzzle.getUniqueSymetric(state.recolor(recolorMask)));
        }
        void init(std::string allowed = "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'", std::string prefix = "") {
            this->puzzle = Puzzle3x3().getPiecePuzzle();
            this->puzzle.solvedState = recolorMask;
            this->puzzle.state = recolorMask;
            this->puzzle.getPiecePuzzle();
            recolorMask = this->puzzle.solvedState;
            std::cout << recolorMask.toString(9, 5);
            this->puzzle.keepOnlyMoves(allowed);
            this->puzzle.generateSymetryTable();
            this->depth = 10;
            this->hashSize = 33;
            this->cfg = cfg;
            this->path = prefix + "MaskExperiment.table";
        }
        MaskExperiment() { init(); }
    };
    
    struct Mask3Color : public PruningStates<Puzzle3x3, 0, useSym, false> {
        State recolorMask = {
            0,  1,  2,  3,  4, 3,  2,  1,  0,  //
            27, 28, 29, 12, 4, 12, 29, 28, 27, //
            36, 37, 20, 41, 4, 41, 20, 37, 36, //
            27, 28, 29, 12, 4, 12, 29, 28, 27, //
            36, 37, 20, 41, 4, 41, 20, 37, 36, //
            0,  1,  2,  3,  4, 3,  2,  1,  0,  //
        };
        int cannotBeSolvedInLimit(int movesAvailable, const State& state) {
            return PruningStates::cannotBeSolvedInLimit(
                movesAvailable, puzzle.getUniqueSymetric(state.recolor(recolorMask))
            );
        }
        void init(std::string allowed = "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'", std::string prefix = "") {
            this->puzzle = Puzzle3x3().getPiecePuzzle();
            this->puzzle.solvedState = recolorMask;
            this->puzzle.state = recolorMask;
            this->puzzle.keepOnlyMoves(allowed);
            this->puzzle.generateSymetryTable();
            this->depth = 11;
            this->hashSize = 35;
            this->cfg = cfg;
            this->path = prefix + "Mask3Color.table";
        }
        Mask3Color() { init(); }
    };
}; // namespace PruningFor3x3

template <bool HAS_SLICES = false, bool HAS_WIDE = false, bool HAS_DOUBLE = false> // 0 is htm, 1 is stm, 2 is qtm
class Solver3x3Restricted : public Solver {
private:
    string allowedMoves = "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'";

public:
    RedundancyTable redundancyTable;
    RedundancyTable redundancyTableInverse;

    // PruningStates<2> pruningTableClassic;
    PruningFor3x3::Mask3Color pruning3Color;
    PruningFor3x3::MaskPermutaion classicTable;
    PruningFor3x3::MaskExperiment experimentTable;

    Solver3x3Restricted() : Solver() {
        if (HAS_SLICES)
            allowedMoves = "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B' RL RL2 RL' UD UD2 UD' FB FB2 FB'";
        else
            allowedMoves = "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'";
        puzzle = Puzzle3x3().getPiecePuzzle();
        puzzle->keepOnlyMoves(allowedMoves);

        // pruningTableClassic.puzzle = puzzle;

        // pruningTableClassic.depth = 7; // 8 HTM 10 rfu
        // pruningTableClassic.hashSize = 27;
        // pruningTableClassic.path = "";
        // pruningTableClassic.cfg = cfg;
    }

    int progress1 = 0;
    int progress2 = 0;

    inline virtual void cancel() {
        Solver::cancel();
        pruning3Color.cancel();
        classicTable.cancel();
        experimentTable.cancel();
    }

    void init() {
        redundancyTable.unload();
        redundancyTable.depth = 3; //3
        redundancyTable.puzzle = *puzzle;
        redundancyTable.cfg = cfg;

        pruning3Color.cfg = cfg;
        classicTable.cfg = cfg;
        experimentTable.cfg = cfg;

        progress1 = 0;
        progress2 = 0;

        tableProgressCallback(0);

        pruning3Color.progressCallback = [&](int p) {
            progress1 = p;
            tableProgressCallback((progress1 + progress2) / 2);
        };
        classicTable.progressCallback = [&](int p) {
            progress2 = p;
            tableProgressCallback((progress1 + progress2) / 2);
        };
        experimentTable.progressCallback = [&](int p) {
            tableProgressCallback(p);
        };

        int hashSize = 0;
        for (hashSize = 0; hashSize < 64; hashSize++) {
            double testmem = pow(2, hashSize) / 1000000000.0;
            if (testmem > cfg->maxMemoryInGb / 2) { break; }
        }

        std::string prefix = to_string(hashSize);

        prefix += HAS_SLICES ? "_slice_" : "_noslice_";

        pruning3Color.init(allowedMoves, prefix);
        classicTable.init(allowedMoves, prefix);
        experimentTable.init(allowedMoves, prefix);

        classicTable.hashSize = hashSize;
        pruning3Color.hashSize = hashSize + 2;
        experimentTable.hashSize = hashSize;

        if constexpr (HAS_SLICES) {
            pruning3Color.depth -= 1;
            classicTable.depth -= 1;
            experimentTable.depth -= 1;
        }

        if (hashSize > 37) {
            pruning3Color.depth += 1;
            classicTable.depth += 1;
            experimentTable.depth -= 1;
        }

        if (hashSize < 32) {
            pruning3Color.depth -= 1;
            classicTable.depth -= 1;
            experimentTable.depth -= 1;
        }

        if (hashSize < 28) {
            pruning3Color.depth -= 1;
            classicTable.depth -= 1;
            experimentTable.depth -= 1;
        }

        redundancyTable.load();
        // pruningTableClassic.load();
        pruning3Color.load();
        // pruning3ColorB.load();
        // pruningRing.load();
        classicTable.load();
        experimentTable.load();
    }

    void deinit() {
        redundancyTable.unload();
        pruning3Color.unload();
        classicTable.unload();
        experimentTable.unload();
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
        printStat("classicTable.getStats()", classicTable.getStats());
        printStat("experimentTable.getStats()", experimentTable.getStats());

        return ss.str();
    }
    Puzzle3x3 sym = Puzzle3x3().getPiecePuzzle();


    virtual State preInsertTransformation(State s) {
        State so = sym.getStandardOrientation(s);
        return sym.getUniqueSymetric(so);
    }

    virtual Puzzle preSolveTransform(Puzzle input) {
        // do more testing here compare stock piece state to new one
        Puzzle3x3 ref = sym;
        ref.solvedState = input.solvedState;
        ref.state = input.state;

        if (ref.getPiecePuzzle().solvedState != sym.solvedState) throw std::runtime_error("cannot use this solver");
        ref = ref.getPiecePuzzle();
        ref.copyMoves(input);
        // cout << ref.getPiecePuzzle().state.toString() << endl;
        // cout << s2.toString() << endl;
        // exit(0);
        return ref;
    }

    virtual bool canDiscardMoves(int movesAvailable, const vector<int>& moves) {
        return redundancyTable.contains(moves);
    }

    virtual bool canDiscardPosition(int movesAvailable, const State& stateReal) {
        if (movesAvailable <= 2) return false;
        if constexpr (HAS_SLICES || HAS_WIDE) {
            State so = sym.getStandardOrientation(stateReal);
            if (pruning3Color.cannotBeSolvedInLimit(movesAvailable, so) == 1) return true;
            if (classicTable.cannotBeSolvedInLimit(movesAvailable, so) == 1) return true;
            return false;
        } else {
            if (pruning3Color.cannotBeSolvedInLimit(movesAvailable, stateReal) == 1) return true;
            if (classicTable.cannotBeSolvedInLimit(movesAvailable, stateReal) == 1) return true;
            return false;
        }
    }
};

class Solver3x3Universal : public Solver {
private:
    string allowedMoves = "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'";
    PruningStates<Puzzle, 2, false, false> pruningTableClassic;
    RedundancyTable redundancyTable;

public:
    Solver3x3Universal() : Solver() { puzzle = Puzzle3x3().getPiecePuzzle(); }

    inline virtual void cancel() {
        Solver::cancel();
        pruningTableClassic.cancel();
    }

    void init() {
        tableProgressCallback(0);
        if (pruningTableClassic.puzzle.getMoves() != puzzle->getMoves() ||
            pruningTableClassic.puzzle.solvedState != puzzle->solvedState) {
            cout << "DEINIT\n";
            deinit();
        }
        pruningTableClassic.puzzle = *puzzle;

        pruningTableClassic.depth = 7; // 8 HTM 10 rfu
        pruningTableClassic.hashSize = 27;
        pruningTableClassic.path = "";
        pruningTableClassic.cfg = cfg;

        redundancyTable.depth = 3; //3
        redundancyTable.puzzle = *puzzle;
        redundancyTable.cfg = cfg;

        pruningTableClassic.progressCallback = [&](int p) { tableProgressCallback(p); };

        redundancyTable.load();
        pruningTableClassic.load();
    }

    void deinit() {
        redundancyTable.unload();
        pruningTableClassic.unload();
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

        return ss.str();
    }

    virtual bool canDiscardMoves(int movesAvailable, const vector<int>& moves) {
        return redundancyTable.contains(moves);
    }

    virtual bool canDiscardPosition(int movesAvailable, const State& stateReal) {
        if (movesAvailable <= 2) return false;
        if (pruningTableClassic.cannotBeSolvedInLimit(movesAvailable, stateReal) == 1) return true;
        return false;
    }
};

class Solver3x3 : public SolverAutoSelector {
    Solver3x3Restricted<0> HTM;
    Solver3x3Restricted<0, 1> WHTM;
    Solver3x3Restricted<1> STM;
    Solver3x3Universal universalSolver;
    double oldMaxGb = -1.0;
    virtual void selectSolver(Puzzle initial) {
        if (cfg->maxMemoryInGb != oldMaxGb) {
            oldMaxGb = cfg->maxMemoryInGb;
            universalSolver.deinit();
            HTM.deinit();
            WHTM.deinit();
            STM.deinit();
        }

        HTM.cfg = cfg;
        STM.cfg = cfg;
        WHTM.cfg = cfg;
        universalSolver.cfg = cfg;

        Puzzle3x3 ref = Puzzle3x3().getPiecePuzzle();
        ref.solvedState = initial.solvedState;
        ref.state = initial.state;

        auto containsAnyMove = [&](std::string moves) {
            auto mz = estd::string_util::splitAll(moves, " ", false);
            for (auto& m : mz) {
                if (initial.getMoves().count(m)) return true;
            }
            return false;
        };

        if (ref.getPiecePuzzle().solvedState != Puzzle3x3().getPiecePuzzle().solvedState) {
            selected = &universalSolver;
        } else if (containsAnyMove("M M2 M' E E2 E' S S2 S'")) {
            selected = &STM;
        } else if (containsAnyMove("r r2 r' l l2 l' u u2 u' d d2 d' f f2 f' b b2 b'")) {
            selected = &WHTM;
        } else {
            selected = &HTM;
        }
    }

public:
    virtual std::string printTableStats() {
        return selected->printTableStats();
    }
    // Solver3x3(std::string s) : Solver3x3() {}
    Solver3x3() {}
};
