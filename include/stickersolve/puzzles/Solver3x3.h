#pragma once

#include <regex>
#include <stickersolve/puzzles/Puzzle3x3.h>
#include <stickersolve/solver/PuzzleSolver.h>

using namespace std;

namespace PruningFor3x3 {
    struct MaskCorners : public PruningStates {
    public:
        State puzzleMask = {
            1, 0, 1, 0, 0, 0, 1, 0, 1, //
            1, 0, 1, 0, 0, 0, 1, 0, 1, //
            1, 0, 1, 0, 0, 0, 1, 0, 1, //
            1, 0, 1, 0, 0, 0, 1, 0, 1, //
            1, 0, 1, 0, 0, 0, 1, 0, 1, //
            1, 0, 1, 0, 0, 0, 1, 0, 1, //
        };
        int opp[6] = {5, 3, 4, 1, 2, 0};
        inline State preHashTransformation(State s) {
            int B = s[35];
            int L = s[42];
            int D = s[51];
            s.inverseRecolor({opp[D], opp[B], opp[L], B, L, D});

            return s.applyMask(puzzleMask);
        }
        bool cannotBeSolvedInLimit(int movesAvailable, const State& state) {
            return PruningStates::cannotBeSolvedInLimit(movesAvailable, preHashTransformation(state));
        }
        MaskCorners(estd::joint_ptr<SolverConfig> cfg = nullptr) {
            this->puzzle = Puzzle3x3("U U2 U' R R2 R' F F2 F'");
            this->puzzle.solvedState = this->puzzle.solvedState.applyMask(puzzleMask);
            this->puzzle.state = this->puzzle.solvedState;
            this->depth = 9; // could generate to 11, but not very useful
            this->hashSize = 25;
            this->cfg = cfg;
            this->path = "MaskCorners.table";
        }
    };
    struct MaskEdges : public PruningStates {
    public:
        vector<int> recolorMask = {0, 1, 2, 1, 2, 3};

        // vector<int> recolorMask = {0, 1, 2, 1, 2, 0};
        // --------------------
        // solver.pruningEdges.getStats()
        // --------------------
        // maxDepth = 9
        // Depth 0 has 1.
        // Depth 1 has 9.
        // Depth 2 has 120.
        // Depth 3 has 1569.
        // Depth 4 has 21071.
        // Depth 5 has 260381.
        // Depth 6 has 2911353.
        // Depth 7 has 23784245.
        // Depth 8 has 38605504.
        // Depth 9 has 910704.
        // Depth 10 has 470375955.
        // 0.066495 Gb / 0.470376 Gb
        State puzzleMask = {
            0, 1, 0, 1, 0, 1, 0, 1, 0, //
            0, 1, 0, 1, 0, 1, 0, 1, 0, //
            0, 1, 0, 1, 0, 1, 0, 1, 0, //
            0, 1, 0, 1, 0, 1, 0, 1, 0, //
            0, 1, 0, 1, 0, 1, 0, 1, 0, //
            0, 1, 0, 1, 0, 1, 0, 1, 0, //
        };
        inline State preHashTransformation(State s) {
            s.recolor(recolorMask);
            return s.applyMask(puzzleMask);
        }
        // bool cannotBeSolvedInLimit(int movesAvailable, const State& state) {
        //     return PruningStates::cannotBeSolvedInLimit(movesAvailable, preHashTransformation(state));
        // }
        static inline State orient1(State s) {
            s.recolor({4, 1, 0, 3, 5, 2});
            static State rotate = Puzzle3x3().getMove("z'");
            return s + rotate;
        }
        static inline State orient2(State s) {
            s.recolor({0, 2, 3, 4, 1, 5});
            static State rotate = Puzzle3x3().getMove("y'");
            return s + rotate;
        }

        bool cannotBeSolvedInLimit(int movesAvailable, State state) {
            State s = preHashTransformation(state);
            if (PruningStates::cannotBeSolvedInLimit(movesAvailable, s)) return true;
            s = preHashTransformation(orient1(state));
            if (PruningStates::cannotBeSolvedInLimit(movesAvailable, s)) return true;
            s = preHashTransformation(orient2(state));
            if (PruningStates::cannotBeSolvedInLimit(movesAvailable, s)) return true;
            return false;
        }
        MaskEdges(estd::joint_ptr<SolverConfig> cfg = nullptr) {
            this->puzzle = Puzzle3x3{
                "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'  FB FB2 FB'  UD UD2 UD'  RL RL2 RL'",
            };
            this->puzzle = Puzzle3x3{
                "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'",
            };
            this->puzzle.solvedState.recolor(recolorMask);
            this->puzzle.solvedState = this->puzzle.solvedState.applyMask(puzzleMask);
            this->puzzle.state = this->puzzle.solvedState;
            this->depth = 9;
            this->hashSize = 29;
            this->cfg = cfg;
            this->path = "MaskEdges.table";
        }
    };
    struct Mask3Color : public PruningStates {
    public:
        std::vector<int> recolorMask = {0, 1, 2, 1, 2, 0};
        inline State preHashTransformation(State s) {
            s.recolor(recolorMask);
            return s;
        }
        bool cannotBeSolvedInLimit(int movesAvailable, const State& state) {
            return PruningStates::cannotBeSolvedInLimit(movesAvailable, preHashTransformation(state));
        }
        Mask3Color(estd::joint_ptr<SolverConfig> cfg = nullptr) {
            this->puzzle = Puzzle3x3{
                "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'  FB FB2 FB'  UD UD2 UD'  RL RL2 RL'",
            };
            this->puzzle = Puzzle3x3{
                "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'",
            };
            this->puzzle.solvedState.recolor(recolorMask);
            this->puzzle.state = this->puzzle.solvedState;
            // this->depth = 9;
            // this->hashSize = 34;
            this->depth = 9;
            this->hashSize = 31;
            this->cfg = cfg;
            this->path = "Mask3Color.table";
        }
    };
    struct MaskOppFaces : public PruningStates {
    public:
        // --------------------
        // solver.pruningOppFaces.getStats()
        // --------------------
        // maxDepth = 9
        // Depth 0 has 1.
        // Depth 1 has 6.
        // Depth 2 has 84.
        // Depth 3 has 1225.
        // Depth 4 has 18174.
        // Depth 5 has 265190.
        // Depth 6 has 3615384.
        // Depth 7 has 39335264.
        // Depth 8 has 163712751.
        // Depth 9 has 53706958.
        // Depth 10 has 1886828611.
        // 0.260655 Gb / 2.14748 Gb = 0.121377
        State puzzleMask = {
            1, 1, 1, 1, 1, 1, 1, 1, 1, // sandwitch mask
            1, 1, 1, 0, 0, 0, 1, 1, 1, // XXX
            1, 1, 1, 0, 0, 0, 1, 1, 1, // XXX X
            1, 1, 1, 0, 0, 0, 1, 1, 1, // OOO O
            1, 1, 1, 0, 0, 0, 1, 1, 1, // XXX X
            1, 1, 1, 1, 1, 1, 1, 1, 1, //
        };
        vector<int> recolorMask = {0, 1, 1, 1, 1, 0};
        State preHashTransformation(State s) {
            s.recolor(recolorMask);
            return s.applyMask(puzzleMask);
        }

        virtual void insert(State s, int moves) { PruningStates::insert(s.applyMask(puzzleMask), moves); }

        static inline State orient1(State s) {
            s.recolor({4, 1, 0, 3, 5, 2});
            static State rotate = Puzzle3x3().getMove("z'");
            return s + rotate;
        }
        static inline State orient2(State s) {
            s.recolor({0, 2, 3, 4, 1, 5});
            static State rotate = Puzzle3x3().getMove("y'");
            return s + rotate;
        }

        bool cannotBeSolvedInLimit(int movesAvailable, State state) {
            State s;
            s = preHashTransformation(state);
            if (PruningStates::cannotBeSolvedInLimit(movesAvailable, s)) return true;
            s = preHashTransformation(orient1(state));
            if (PruningStates::cannotBeSolvedInLimit(movesAvailable, s)) return true;
            s = preHashTransformation(orient2(state));
            if (PruningStates::cannotBeSolvedInLimit(movesAvailable, s)) return true;
            return false;
        }

        MaskOppFaces(estd::joint_ptr<SolverConfig> cfg = nullptr) {
            // this->puzzle = Puzzle3x3{
            //     "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'  FB FB2 FB'  UD UD2 UD'  RL RL2 RL'",
            // };
            this->puzzle = Puzzle3x3{
                "U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'",
            };
            this->puzzle.solvedState.recolor(recolorMask);
            this->puzzle.state = this->puzzle.solvedState;
            this->depth = 8;
            this->hashSize = 31;
            this->cfg = cfg;
            this->path = "MaskOppFaces.table";
        }
    };
}; // namespace PruningFor3x3

class Solver3x3 : public Solver {
public:
    RedundancyTable redundancyTable;
    RedundancyTable redundancyTableInverse;
    // RecoloredPruningStates pruningTableRecolored;

    PruningStates pruningTableClassic;
    PruningFor3x3::MaskCorners pruningCorners;
    PruningFor3x3::MaskEdges pruningEdges;
    PruningFor3x3::Mask3Color pruning3Color;
    PruningFor3x3::MaskOppFaces pruningOppFaces;

    Solver3x3() : Solver3x3("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'"){};
    Solver3x3(string allowedMoves) : Solver() {
        puzzle = Puzzle3x3(allowedMoves);

        redundancyTable.depth = 3;
        redundancyTable.puzzle = puzzle;
        redundancyTable.cfg = cfg;

        pruningCorners.cfg = cfg;
        pruningEdges.cfg = cfg;
        pruning3Color.cfg = cfg;
        pruningTableClassic.cfg = cfg;
        pruningOppFaces.cfg = cfg;

        pruningTableClassic.puzzle = puzzle;

        pruningTableClassic.depth = 7; // 8 HTM 10 rfu
        pruningTableClassic.hashSize = 27;
        pruningTableClassic.path = "pruningNormal.table";
        pruningTableClassic.cfg = cfg;
    }

    vector<State> rotationMapping = vector<State>(36);
    vector<vector<int>> recolorMapping = vector<vector<int>>(36);

    inline int orientationToInt(int t, int f) { return 6 * t + f; }

    const int top = 4;
    const int bottom = 49;
    const int left = 40;
    const int right = 22;
    const int front = 13;
    const int back = 31;

    const int topColor = Puzzle3x3()[top];
    const int bottomColor = Puzzle3x3()[bottom];
    const int leftColor = Puzzle3x3()[left];
    const int rightColor = Puzzle3x3()[right];
    const int frontColor = Puzzle3x3()[front];
    const int backColor = Puzzle3x3()[back];

    State rotateX;
    State rotateX2;
    State rotateX3;
    vector<int> recolorX;
    vector<int> recolorX2;
    vector<int> recolorX3;

    State rotateY;
    State rotateY2;
    State rotateY3;
    vector<int> recolorY;
    vector<int> recolorY2;
    vector<int> recolorY3;

    State rotateZ;
    State rotateZ2;
    State rotateZ3;
    vector<int> recolorZ;
    vector<int> recolorZ2;
    vector<int> recolorZ3;

    void init() {
        redundancyTable.load();
        // pruningTableRecolored.load();
        pruningTableClassic.load();
        pruningCorners.load();
        pruningEdges.load();
        pruning3Color.load();
        pruningOppFaces.load();

        Puzzle pzl = Puzzle3x3();

        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 4; j++) {
                State rot;
                if (i == 0) rot = pzl.getMove("x2") + pzl.getMove("x2"); // nop move
                if (i == 1) rot = pzl.getMove("x2");
                if (i == 2) rot = pzl.getMove("x");
                if (i == 3) rot = pzl.getMove("x'");
                if (i == 4) rot = pzl.getMove("z");
                if (i == 5) rot = pzl.getMove("z'");

                if (j == 1) rot += pzl.getMove("y");
                if (j == 2) rot += pzl.getMove("y2");
                if (j == 3) rot += pzl.getMove("y'");

                State tst = pzl;
                tst -= rot;

                rotationMapping[orientationToInt(tst[top], tst[front])] = rot;

                vector<int> recolorMap = {0, 0, 0, 0, 0, 0};

                State solved = pzl;

                recolorMap[tst[top]] = solved[top];
                recolorMap[tst[bottom]] = solved[bottom];
                recolorMap[tst[left]] = solved[left];
                recolorMap[tst[right]] = solved[right];
                recolorMap[tst[front]] = solved[front];
                recolorMap[tst[back]] = solved[back];

                recolorMapping[orientationToInt(tst[top], tst[front])] = recolorMap;
            }
        }

        rotateX = State(pzl.getMove("x"));
        rotateX2 = State(pzl.getMove("x2"));
        rotateX3 = State(pzl.getMove("x'"));
        recolorX = getRecolorMapForMove(rotateX);
        recolorX2 = getRecolorMapForMove(rotateX2);
        recolorX3 = getRecolorMapForMove(rotateX3);

        rotateY = State(pzl.getMove("y"));
        rotateY2 = State(pzl.getMove("y2"));
        rotateY3 = State(pzl.getMove("y'"));
        recolorY = getRecolorMapForMove(rotateY);
        recolorY2 = getRecolorMapForMove(rotateY2);
        recolorY3 = getRecolorMapForMove(rotateY3);

        rotateZ = State(pzl.getMove("z"));
        rotateZ2 = State(pzl.getMove("z2"));
        rotateZ3 = State(pzl.getMove("z'"));
        recolorZ = getRecolorMapForMove(rotateZ);
        recolorZ2 = getRecolorMapForMove(rotateZ2);
        recolorZ3 = getRecolorMapForMove(rotateZ3);
    }
    //rotatePuzzleToStandardOrientation
    State rotatePuzzleSO(State s) { return s + rotationMapping[orientationToInt(s[top], s[front])]; }

    //recolorPuzzleToStandardOrientation
    State recolorPuzzleSO(State s) {
        s.recolor(recolorMapping[orientationToInt(s[top], s[front])]);
        return s;
    }

    //getRecolorMapForMove
    vector<int> getRecolorMapForMove(State m) {
        Puzzle pzl = Puzzle3x3();
        State state = Puzzle3x3();
        state += m;
        return recolorMapping[orientationToInt(state[top], state[front])];
    }

    virtual bool canDiscardMoves(int movesAvailable, const vector<int>& moves) {
        return redundancyTable.contains(moves);
    }

    virtual bool canDiscardPosition(int movesAvailable, const State& stateReal) {
        if (movesAvailable <= 2) return false;
        if (movesAvailable >= 10) return false;
        State s = rotatePuzzleSO(stateReal);

        if (pruningTableClassic.cannotBeSolvedInLimit(movesAvailable, s)) return true;

        if (pruning3Color.cannotBeSolvedInLimit(movesAvailable, s)) return true;
        if (pruningOppFaces.cannotBeSolvedInLimit(movesAvailable, s)) return true;
        if (pruningEdges.cannotBeSolvedInLimit(movesAvailable, s)) return true;
        if (pruningCorners.cannotBeSolvedInLimit(movesAvailable, s)) return true;
        return false;
    }
};
