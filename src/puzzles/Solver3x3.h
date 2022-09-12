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

		s.recolor({0,1,1,1,1,2});
		// Depth 0 has 1.
		// Depth 1 has 18.
		// Depth 2 has 295.
		// Depth 3 has 4814.
		// Depth 4 has 78836.
		// Depth 5 has 1288318.
		// Depth 6 has 20911358.
		// Depth 7 has 329915476.
		// Depth 8 has 4320443393.
		// Depth 9 has 12507226675.
		//s.recolor({0,1,2,1,2,3});
		// Depth 0 has 1.
		// Depth 1 has 21.
		// Depth 2 has 359.
		// Depth 3 has 5937.
		// Depth 4 has 98828.
		// Depth 5 has 1640953.
		// Depth 6 has 27256485.
		// Depth 7 has 445602386.
		// Depth 8 has 5888972610.
		// Depth 9 has 10816291604.

        //return s.applyMask(puzzleMask);
		return s;
    }
};

class Puzzle3x3: public Puzzle{
public:
	Puzzle3x3(): Puzzle(
		{
			0, 0, 0,
			0, 0, 0,
			0, 0, 0,

			1, 1, 1,
			1, 1, 1,
			1, 1, 1,

			2, 2, 2,
			2, 2, 2,
			2, 2, 2,

			3, 3, 3,
			3, 3, 3,
			3, 3, 3,

			4, 4, 4,
			4, 4, 4,
			4, 4, 4,

			5, 5, 5,
			5, 5, 5,
			5, 5, 5
		}
	){

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
			this->addMove( "U", move );
			this->addMove( "U2", move * 2 );
			this->addMove( "U'", move * 3 );
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
			this->addMove( "R", move );
			this->addMove( "R2", move * 2 );
			this->addMove( "R'", move * 3 );
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
			this->addMove( "F", move );
			this->addMove( "F2", move * 2 );
			this->addMove( "F'", move * 3 );

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
			this->addMove( "L", move );
			this->addMove( "L2", move * 2 );
			this->addMove( "L'", move * 3 );
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
			this->addMove( "D", move );
			this->addMove( "D2", move * 2 );
			this->addMove( "D'", move * 3 );
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
			this->addMove( "B", move );
			this->addMove( "B2", move * 2 );
			this->addMove( "B'", move * 3 );
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
			this->addMove( "M'", move );
			this->addMove( "M2", move * 2 );
			this->addMove( "M", move * 3 );
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
			this->addMove( "S", move );
			this->addMove( "S2", move * 2 );
			this->addMove( "S'", move * 3 );
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
			this->addMove( "E'", move );
			this->addMove( "E2", move * 2 );
			this->addMove( "E", move * 3 );
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

			this->addMove( "r", move );
			this->addMove( "r2", move * 2 );
			this->addMove( "r'", move * 3 );
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
			this->addMove( "f", move );
			this->addMove( "f2", move * 2 );
			this->addMove( "f'", move * 3 );
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
			this->addMove( "u", move );
			this->addMove( "u2", move * 2 );
			this->addMove( "u'", move * 3 );
		}

		{
			State move = this->getMove("B");
			move += this->getMove("S'");
			this->addMove("b", move);
			this->addMove("b2", move * 2 );
			this->addMove("b'", move * 3 );
		}
		{
			State move = this->getMove("D");
			move += this->getMove("E");
			this->addMove("d", move);
			this->addMove("d2", move * 2 );
			this->addMove("d'", move * 3 );
		}
		{
			State move = this->getMove("L");
			move += this->getMove("M");
			this->addMove("l", move);
			this->addMove("l2", move * 2 );
			this->addMove("l'", move * 3 );
		}

		{
			State move = this->getMove("R");
			move += this->getMove("M'");
			move += this->getMove("L'");
			this->addMove("x", move);
			this->addMove("x2", move * 2 );
			this->addMove("x'", move * 3 );
		}
		{
			State move = this->getMove("F");
			move += this->getMove("S");
			move += this->getMove("B'");
			this->addMove("z", move);
			this->addMove("z2", move * 2 );
			this->addMove("z'", move * 3 );
		}
		{
			State move = this->getMove("U");
			move += this->getMove("E'");
			move += this->getMove("D'");
			this->addMove("y", move);
			this->addMove("y2", move * 2 );
			this->addMove("y'", move * 3 );
		}
		{
			State move = this->getMove("U") + this->getMove("D'");
			this->addMove("UD", move);
			this->addMove("UD2", move * 2 );
			this->addMove("UD'", move * 3 );
		}
		{
			State move = this->getMove("R") + this->getMove("L'");
			this->addMove("RL", move);
			this->addMove("RL2", move * 2 );
			this->addMove("RL'", move * 3 );
		}
		{
			State move = this->getMove("F") + this->getMove("B'");
			this->addMove("FB", move);
			this->addMove("FB2", move * 2 );
			this->addMove("FB'", move * 3 );
		}
	}

	Puzzle3x3(string allowed): Puzzle3x3(){
		keepOnlyMoves(allowed);
	}
};

class Solver3x3: public Solver{
public:
	RedundancyTable redundancyTable;
	MaskedPruningStates pruningTableMasked;
	PruningStates pruningTableClassic;

	Solver3x3(): Solver3x3("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'"){
		
	};
	Solver3x3(string allowedMoves): Solver(){
		puzzle = Puzzle3x3(allowedMoves);
		
		
		// maxDepth = 8
		// Depth 0 has 1.
		// Depth 1 has 15.
		// Depth 2 has 237.
		// Depth 3 has 3924.
		// Depth 4 has 61753.
		// Depth 5 has 921960.
		// Depth 6 has 12442459.
		// Depth 7 has 133098628.
		// Depth 8 has 707220312.
		// Depth 9 has 16326119895.
		// Solving took: 12.7945 seconds.

		// pruningTable.puzzleMask = {
        //     0,0,0, 0,0,0, 0,0,0,
        //     0,0,0, 0,0,0, 0,0,0,
        //     0,0,0, 0,1,1, 0,1,1,
        //     0,0,0, 1,1,1, 1,1,1,
        //     0,0,0, 1,1,0, 1,1,0,
        //     0,0,0, 1,1,1, 1,1,1
        // };

		// pruningTable.puzzleMask = {
        //     0,0,0, 0,0,0, 0,0,0,
        //     0,0,0, 0,0,0, 1,1,1,
        //     0,0,0, 0,0,0, 1,1,1,
        //     0,0,0, 0,0,0, 1,1,1,
        //     0,0,0, 0,0,0, 1,1,1,
        //     1,1,1, 1,1,1, 1,1,1
        // };

		pruningTableMasked.puzzleMask = {
            1,1,1, 1,1,1, 1,1,1,
            0,0,0, 0,0,0, 0,0,0,
            0,0,0, 0,0,0, 0,0,0,
            0,0,0, 0,0,0, 0,0,0,
            0,0,0, 0,0,0, 0,0,0,
            1,1,1, 1,1,1, 1,1,1
        };

		pruningTableMasked.puzzle = Puzzle3x3("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'    FB FB2 FB'  UD UD2 UD'  RL RL2 RL'");

		pruningTableMasked.maxSizeInGb = 34.0;
		pruningTableMasked.depth = 9;// 8 HTM 10 rfu
		pruningTableMasked.hashSize = 34;
		pruningTableMasked.path = cfg->pruiningTablesPath+"/pruning.table";
		pruningTableMasked.useMmap = false;
		pruningTableMasked.log.forward(cerr);
		pruningTableMasked.cfg = cfg;

		pruningTableClassic.puzzle = pruningTableMasked.puzzle;

		pruningTableClassic.maxSizeInGb = 34.0;
		pruningTableClassic.depth = 5;// 8 HTM 10 rfu
		pruningTableClassic.hashSize = 23;
		pruningTableClassic.path =  cfg->pruiningTablesPath+"/pruningNormal.table";
		pruningTableClassic.useMmap = false;
		pruningTableClassic.log.forward(cerr);
		pruningTableClassic.cfg = cfg;

		log.forward(cerr);
		redundancyTable.depth = 3;
		//redundancyTable.path = cfg->pruiningTablesPath+"/redundancy.table";
		redundancyTable.puzzle = puzzle;
		redundancyTable.log.forward(cerr);
		redundancyTable.cfg = cfg;

		pruningTableMasked.redundancyTableInverse.depth = 5;
		pruningTableMasked.redundancyTableInverse.path = cfg->pruiningTablesPath+"/redundancyInverse.table";
		pruningTableMasked.redundancyTableInverse.inverse = true;
		pruningTableMasked.redundancyTableInverse.puzzle = pruningTableMasked.puzzle;
		pruningTableMasked.redundancyTableInverse.log.forward(cerr);
		pruningTableMasked.redundancyTableInverse.cfg = cfg;
		pruningTableClassic.redundancyTableInverse = pruningTableMasked.redundancyTableInverse;

	}
	
	vector<State> rotationMapping = vector<State>(36);
	vector<vector<int>> recolorMapping = vector<vector<int>>(36);

	inline int orientationToInt(int t, int f){
		return 6*t+f;
	}

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
 		pruningTableMasked.load();
		pruningTableClassic.load();

		Puzzle pzl = Puzzle3x3();

		for(int i = 0; i < 6; i++){
			for(int j = 0; j < 4; j++){
				State rot;
				if(i == 0) rot = pzl.getMove("x2") + pzl.getMove("x2"); // nop move
				if(i == 1) rot = pzl.getMove("x2");
				if(i == 2) rot = pzl.getMove("x");
				if(i == 3) rot = pzl.getMove("x'");
				if(i == 4) rot = pzl.getMove("z");
				if(i == 5) rot = pzl.getMove("z'");

				if(j == 1) rot += pzl.getMove("y");
				if(j == 2) rot += pzl.getMove("y2");
				if(j == 3) rot += pzl.getMove("y'");

				State tst = pzl;
				tst -= rot;
				
				rotationMapping[orientationToInt(tst[top], tst[front])] = rot;

				vector<int> recolorMap = {0,0,0,0,0,0};

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
	State rotatePuzzleSO(State s){
		return s + rotationMapping[orientationToInt(s[top], s[front])];
	}

	//recolorPuzzleToStandardOrientation	
	State recolorPuzzleSO(State s){
		s.recolor(recolorMapping[orientationToInt(s[top], s[front])]);
		return s;
	}

	//getRecolorMapForMove	
	vector<int> getRecolorMapForMove(State m){
		Puzzle pzl = Puzzle3x3();
		State state = Puzzle3x3();
		state += m;
		return recolorMapping[orientationToInt(state[top], state[front])];
	}

	virtual bool canDiscardMoves(int movesAvailable, const vector<int>& moves){
		return redundancyTable.contains(moves);
	}

	virtual bool canDiscardPosition( int movesAvailable, const State& stateReal ){
		if(pruningTableMasked.cannotUseTable(movesAvailable)) return false;

		State s = rotatePuzzleSO(stateReal);
		if(pruningTableClassic.cannotBeSolvedInLimit(movesAvailable, s)) return true;



        if(pruningTableMasked.cannotBeSolvedInLimit(movesAvailable, s)) return true;

        s += rotateX;
		// s = recolorPuzzleSO(s);
		s.recolor(recolorX);
        if(pruningTableMasked.cannotBeSolvedInLimit(movesAvailable, s)) return true;

		s += rotateZ;
		// s = recolorPuzzleSO(s);
		s.recolor(recolorZ);
        if(pruningTableMasked.cannotBeSolvedInLimit(movesAvailable, s)) return true;


		return false;
	}

};

#endif
