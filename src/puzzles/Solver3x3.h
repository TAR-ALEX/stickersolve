#pragma once

#ifndef SOLVER_3x3_RUFrBD
#define SOLVER_3x3_RUFrBD

#include "../solver/PuzzleSolver.h"
#include <regex>

using namespace std;

struct MaskedPruningStates: public PruningStates {
public:
    State puzzleMask;
    virtual State preHashTransformation(State s) {
        return s.applyMask(puzzleMask);
    }
};

Puzzle Puzzle3x3(){
	Puzzle puzzle( 
		{
			"U", "U", "U",
			"U", "U", "U",
			"U", "U", "U",

			"F", "F", "F",
			"F", "F", "F",
			"F", "F", "F",

			"R", "R", "R",
			"R", "R", "R",
			"R", "R", "R",

			"B", "B", "B",
			"B", "B", "B",
			"B", "B", "B",

			"L", "L", "L",
			"L", "L", "L",
			"L", "L", "L",

			"D", "D", "D",
			"D", "D", "D",
			"D", "D", "D"
		}
	);

// ---------------------------------------------------
//        Move that does nothing
// ---------------------------------------------------
//         State move = {
//             0, 1, 2,      3, 4, 5,      6, 7, 8,
//             9, 10, 11,    12, 13, 14,   15, 16, 17,
//             18, 19, 20,   21, 22, 23,   24, 25, 26,
//             27, 28, 29,   30, 31, 32,   33, 34, 35,
//             36, 37, 38,   39, 40, 41,   42, 43, 44,
//             45, 46, 47,   48, 49, 50,   51, 52, 53
//         };
		
	{
		State move = {
			6, 3, 0,      7, 4, 1,      8, 5, 2,
			18, 19, 20,   12, 13, 14,   15, 16, 17,
			27, 28, 29,   21, 22, 23,   24, 25, 26,
			36, 37, 38,   30, 31, 32,   33, 34, 35,
			9, 10, 11,    39, 40, 41,   42, 43, 44,
			45, 46, 47,   48, 49, 50,   51, 52, 53
		};
		puzzle.addMove( "U", move );
		puzzle.addMove( "U2", move * 2 );
		puzzle.addMove( "U'", move * 3 );
	}
	{
		State move = {
			0, 1, 11,     3, 4, 14,     6, 7, 17,
			9, 10, 47,    12, 13, 50,   15, 16, 53,
			24, 21, 18,   25, 22, 19,   26, 23, 20,
			8, 28, 29,    5, 31, 32,    2, 34, 35,
			36, 37, 38,   39, 40, 41,   42, 43, 44,
			45, 46, 33,   48, 49, 30,   51, 52, 27
		};
		puzzle.addMove( "R", move );
		puzzle.addMove( "R2", move * 2 );
		puzzle.addMove( "R'", move * 3 );
	}
	{
		State move = {
			0, 1, 2,      3, 4, 5,      44, 41, 38,
			15, 12, 9,    16, 13, 10,   17, 14, 11,
			6, 19, 20,    7, 22, 23,    8, 25, 26,
			27, 28, 29,   30, 31, 32,   33, 34, 35,
			36, 37, 45,   39, 40, 46,   42, 43, 47,
			24, 21, 18,   48, 49, 50,   51, 52, 53
		};
		puzzle.addMove( "F", move );
		puzzle.addMove( "F2", move * 2 );
		puzzle.addMove( "F'", move * 3 );

	}
	{
		State move = {
			35, 1, 2, 32, 4, 5, 29, 7, 8,
			0, 10, 11, 3, 13, 14, 6, 16, 17,
			18, 19, 20, 21, 22, 23, 24, 25, 26,
			27, 28, 51, 30, 31, 48, 33, 34, 45,

			42, 39, 36, 43, 40, 37, 44, 41, 38,
			9, 46, 47, 12, 49, 50, 15, 52, 53
		};
		puzzle.addMove( "L", move );
		puzzle.addMove( "L2", move * 2 );
		puzzle.addMove( "L'", move * 3 );
	}
	{
		State move = {
			0, 1, 2, 3, 4, 5, 6, 7, 8,

			9, 10, 11, 12, 13, 14,   42, 43, 44, // <>,
			18, 19, 20, 21, 22, 23,  15, 16, 17, // <>
			27, 28, 29, 30, 31, 32,  24, 25, 26, // <>
			36, 37, 38, 39, 40, 41,  33, 34, 35, // <>

			51, 48, 45, 52, 49, 46, 53, 50, 47
		};
		puzzle.addMove( "D", move );
		puzzle.addMove( "D2", move * 2 );
		puzzle.addMove( "D'", move * 3 );
	}
	{
		State move = {
			20, 23, 26, 3, 4, 5, 6, 7, 8,
			9, 10, 11, 12, 13, 14, 15, 16, 17,
			18, 19, 53, 21, 22, 52, 24, 25, 51,
			33, 30, 27, 34, 31, 28, 35, 32, 29,
			2, 37, 38, 1, 40, 41, 0, 43, 44,
			45, 46, 47, 48, 49, 50, 36, 39, 42
		};
		puzzle.addMove( "B", move );
		puzzle.addMove( "B2", move * 2 );
		puzzle.addMove( "B'", move * 3 );
	}
	{
		State move = {
			0, 10, 2, 3, 13, 5, 6, 16, 8,
			9, 46, 11, 12, 49, 14, 15, 52, 17,
			18, 19, 20, 21, 22, 23, 24, 25, 26,
			27, 7, 29, 30, 4, 32, 33, 1, 35,
			36, 37, 38, 39, 40, 41, 42, 43, 44,
			45, 34, 47, 48, 31, 50, 51, 28, 53
		};
		puzzle.addMove( "M'", move );
		puzzle.addMove( "M2", move * 2 );
		puzzle.addMove( "M", move * 3 );
	}
	{
		State move = {
			0, 1, 2, 43, 40, 37, 6, 7, 8,
			9, 10, 11, 12, 13, 14, 15, 16, 17,
			18, 3, 20, 21, 4, 23, 24, 5, 26,
			27, 28, 29, 30, 31, 32, 33, 34, 35,
			36, 48, 38, 39, 49, 41, 42, 50, 44,
			45, 46, 47, 25, 22, 19, 51, 52, 53
		};
		puzzle.addMove( "S", move );
		puzzle.addMove( "S2", move * 2 );
		puzzle.addMove( "S'", move * 3 );
	}
	{
		State move = {
			0, 1, 2,      3, 4, 5,      6, 7, 8,
			9, 10, 11,    21, 22, 23,   15, 16, 17,
			18, 19, 20,   30, 31, 32,   24, 25, 26,
			27, 28, 29,   39, 40, 41,   33, 34, 35,
			36, 37, 38,   12, 13, 14,   42, 43, 44,
			45, 46, 47,   48, 49, 50,   51, 52, 53
		};
		puzzle.addMove( "E'", move );
		puzzle.addMove( "E2", move * 2 );
		puzzle.addMove( "E", move * 3 );
	}
	{
		State move = {
			0, 10, 11, 3, 13, 14, 6, 16, 17,
			9, 46, 47, 12, 49, 50, 15, 52, 53,
			24, 21, 18, 25, 22, 19, 26, 23, 20,
			8, 7, 29, 5, 4, 32, 2, 1, 35,
			36, 37, 38, 39, 40, 41, 42, 43, 44,
			45, 34, 33, 48, 31, 30, 51, 28, 27
		};

		puzzle.addMove( "r", move );
		puzzle.addMove( "r2", move * 2 );
		puzzle.addMove( "r'", move * 3 );
	}
	{
		State move = {
			0, 1, 2,      43, 40, 37,      44, 41, 38,
			15, 12, 9,    16, 13, 10,   17, 14, 11,
			6, 3, 20,    7, 4, 23,    8, 5, 26,
			27, 28, 29,   30, 31, 32,   33, 34, 35,
			36, 48, 45,   39, 49, 46,   42, 50, 47,
			24, 21, 18,   25, 22, 19,   51, 52, 53
		};
		puzzle.addMove( "f", move );
		puzzle.addMove( "f2", move * 2 );
		puzzle.addMove( "f'", move * 3 );
	}
	{
		State move = {
			6, 3, 0,      7, 4, 1,      8, 5, 2,
			18, 19, 20,   21, 22, 23,   15, 16, 17,
			27, 28, 29,   30, 31, 32,   24, 25, 26,
			36, 37, 38,   39, 40, 41,   33, 34, 35,
			9, 10, 11,    12, 13, 14,   42, 43, 44,
			45, 46, 47,   48, 49, 50,   51, 52, 53
		};
		puzzle.addMove( "u", move );
		puzzle.addMove( "u2", move * 2 );
		puzzle.addMove( "u'", move * 3 );
	}

	{
		State move = puzzle.getMove("B");
		move += puzzle.getMove("S'");
		puzzle.addMove("b", move);
		puzzle.addMove("b2", move * 2 );
		puzzle.addMove("b'", move * 3 );
	}
	{
		State move = puzzle.getMove("D");
		move += puzzle.getMove("E");
		puzzle.addMove("d", move);
		puzzle.addMove("d2", move * 2 );
		puzzle.addMove("d'", move * 3 );
	}
	{
		State move = puzzle.getMove("L");
		move += puzzle.getMove("M");
		puzzle.addMove("l", move);
		puzzle.addMove("l2", move * 2 );
		puzzle.addMove("l'", move * 3 );
	}

	{
		State move = puzzle.getMove("R");
		move += puzzle.getMove("M'");
		move += puzzle.getMove("L'");
		puzzle.addMove("x", move);
		puzzle.addMove("x2", move * 2 );
		puzzle.addMove("x'", move * 3 );
	}
	{
		State move = puzzle.getMove("F");
		move += puzzle.getMove("S");
		move += puzzle.getMove("B'");
		puzzle.addMove("z", move);
		puzzle.addMove("z2", move * 2 );
		puzzle.addMove("z'", move * 3 );
	}
	{
		State move = puzzle.getMove("U");
		move += puzzle.getMove("E'");
		move += puzzle.getMove("D'");
		puzzle.addMove("y", move);
		puzzle.addMove("y2", move * 2 );
		puzzle.addMove("y'", move * 3 );
	}
	{
		State move = puzzle.getMove("U") + puzzle.getMove("D'");
		puzzle.addMove("UD", move);
		puzzle.addMove("UD2", move * 2 );
		puzzle.addMove("UD'", move * 3 );
	}
	{
		State move = puzzle.getMove("R") + puzzle.getMove("L'");
		puzzle.addMove("RL", move);
		puzzle.addMove("RL2", move * 2 );
		puzzle.addMove("RL'", move * 3 );
	}
	{
		State move = puzzle.getMove("F") + puzzle.getMove("B'");
		puzzle.addMove("FB", move);
		puzzle.addMove("FB2", move * 2 );
		puzzle.addMove("FB'", move * 3 );
	}
	return puzzle;
}

Puzzle Puzzle3x3(string allowed){
	Puzzle puzzle = Puzzle3x3();
	puzzle.keepOnlyMoves(allowed);
	return puzzle;
}

class Solver3x3: public Solver{
public:
	RedundancyTable redundancyTable;
	MaskedPruningStates pruningTable;

	Solver3x3(): Solver3x3("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'"){
		
	};
	Solver3x3(string allowedMoves): Solver(){
		puzzle = Puzzle3x3(allowedMoves);
		
		pruningTable.puzzleMask = {
            0,0,0, 0,0,0, 0,0,0,
            0,0,0, 0,0,0, 0,0,0,
            0,0,0, 0,1,1, 0,1,1,
            0,0,0, 1,1,1, 1,1,1,
            0,0,0, 1,1,0, 1,1,0,
            1,1,1, 1,1,1, 1,1,1
        };

		pruningTable.puzzle = Puzzle3x3("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'    FB FB2 FB'  UD UD2 UD'  RL RL2 RL'");

		pruningTable.maxSizeInGb = 34.0;
		pruningTable.depth = 7;// 8 HTM 10 rfu
		pruningTable.hashSize = 34;
		pruningTable.path = "../pruning.table";
		pruningTable.useMmap = false;
		pruningTable.log.forward(cerr);
		pruningTable.cfg = cfg;

		log.forward(cerr);
		redundancyTable.depth = 3;
		//redundancyTable.path = "../redundancy.table";
		redundancyTable.puzzle = puzzle;
		redundancyTable.log.forward(cerr);
		redundancyTable.cfg = cfg;

		pruningTable.redundancyTableInverse.depth = 5;
		pruningTable.redundancyTableInverse.path = "../redundancyInverse.table";
		pruningTable.redundancyTableInverse.inverse = true;
		pruningTable.redundancyTableInverse.puzzle = pruningTable.puzzle;
		pruningTable.redundancyTableInverse.log.forward(cerr);
		pruningTable.redundancyTableInverse.cfg = cfg;
		

	}
	void init() {
		redundancyTable.load();
 		pruningTable.load();
	}
	virtual bool canDiscardMoves(int movesAvailable, const vector<int>& moves){
		return redundancyTable.contains(moves);
	}
	virtual bool canDiscardPosition( int movesAvailable, const State& state ){
        State s = state;

        static State rotateX = {
            35,34,33,  32,31,30,  29,28,27,
            0,1,2,     3,4,5,     6,7,8,
            20,23,26,  19,22,25,  18,21,24,
            53,52,51,  50,49,48,  47,46,45,
            42,39,36,  43,40,37,  44,41,38,
            9,10,11,   12,13,14,  15,16,17,
        };

        static State rotateX2 = rotateX*2;
        static State rotateX3 = rotateX*3;

        static vector<int> recolorX = {5, 0, 1, 3, 4, 2};
        static vector<int> recolorX2 = {2, 5, 0, 3, 4, 1};


        static State rotateY = {
            6,3,0,     7,4,1,     8,5,2,
            18,19,20,  21,22,23,  24,25,26,
            27,28,29,  30,31,32,  33,34,35,
            36,37,38,  39,40,41,  42,43,44,
            9,10,11,   12,13,14,  15,16,17,
            47,50,53,  46,49,52,  45,48,51,
        };
        /*
         * 5 W W 5
         * 2 G R 4
         * 4 R B 0
         * 0 B O 3
         * 3 O G 2
         * 1 Y Y 1
         */

        /*
         * 5 W W 5
         * 2 G B 0
         * 4 R O 3
         * 0 B G 2
         * 3 O R 4
         * 1 Y Y 1
         */

        static State rotateY2 = rotateY*2;
        static State rotateY3 = rotateY*3;

        static vector<int> recolorY = {4,1,3,0,2,5};
        static vector<int> recolorY2 = {2,1,0,4,3,5};

        if(pruningTable.cannotBeSolvedInLimit(movesAvailable, pruningTable.preHashTransformation(s))) return true;

        s = state+rotateX2;
        s.recolor(recolorX2);
        if(pruningTable.cannotBeSolvedInLimit(movesAvailable, pruningTable.preHashTransformation(s))) return true;

        s = s+rotateX;
        s.recolor(recolorX);
        if(pruningTable.cannotBeSolvedInLimit(movesAvailable, pruningTable.preHashTransformation(s))) return true;

        s = s+rotateX2;
        s.recolor(recolorX2);
        if(pruningTable.cannotBeSolvedInLimit(movesAvailable, pruningTable.preHashTransformation(s))) return true;



        //--------------------------
        s = state+rotateY;
        s.recolor(recolorY);
        if(pruningTable.cannotBeSolvedInLimit(movesAvailable, pruningTable.preHashTransformation(s))) return true;

        s = state+rotateX2;
        s.recolor(recolorX2);
        if(pruningTable.cannotBeSolvedInLimit(movesAvailable, pruningTable.preHashTransformation(s))) return true;

        s = s+rotateX;
        s.recolor(recolorX);
        if(pruningTable.cannotBeSolvedInLimit(movesAvailable, pruningTable.preHashTransformation(s))) return true;

        s = s+rotateX2;
        s.recolor(recolorX2);
        if(pruningTable.cannotBeSolvedInLimit(movesAvailable, pruningTable.preHashTransformation(s))) return true;



        //--------------------------
        s = state+rotateY2;
        s.recolor(recolorY2);
        if(pruningTable.cannotBeSolvedInLimit(movesAvailable, pruningTable.preHashTransformation(s))) return true;

        s = state+rotateX2;
        s.recolor(recolorX2);
        if(pruningTable.cannotBeSolvedInLimit(movesAvailable, pruningTable.preHashTransformation(s))) return true;

        s = s+rotateX;
        s.recolor(recolorX);
        if(pruningTable.cannotBeSolvedInLimit(movesAvailable, pruningTable.preHashTransformation(s))) return true;

        s = s+rotateX2;
        s.recolor(recolorX2);
        if(pruningTable.cannotBeSolvedInLimit(movesAvailable, pruningTable.preHashTransformation(s))) return true;




		return false;
	}

};

#endif
