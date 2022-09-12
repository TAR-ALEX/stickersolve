#include <iostream>
#include <vector>
#include <chrono>
using namespace std::chrono;

#include "solver/puzzle.h"
#include "puzzles/Solver3x3.h"

using namespace std;


#include <iomanip>
#include "solver/PuzzleSolver.h"
#include <algorithm>
		
int main( int argc, char const* argv[] ) {
	cerr << "----------------------------------------------------------------\n";
	try {
		Solver3x3 solver("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'");

		Solver s;
		// Solver3x3 solver("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'   M M2 M' S S2 S' E E2 E'");
		solver.cfg->targetThreads = 16;
		
		Puzzle p = Puzzle3x3();

		cout << p.toString() << endl;

		int W = 0;
		int G = 1;
		int R = 2;
		int B = 3;
		int O = 4;
		int Y = 5;
		
		int Q = 6;

		// p.applyMoves("U R2 F B R B2 R U2 L B2 R U' D' R2 F R' L B2 U2 F2"); // superflip
		// p.applyMoves("R' L U R2 F D L' U' L2 F2 B2 U' D2 R2 B2 U2 D'");//   R2

		p = {
			W, R, W, W, W, W, W, O, W,
			G, W, G, G, G, G, G, G, G,
			R, B, R, R, R, R, R, R, R,
			B, W, B, B, B, B, B, B, B,
			O, G, O, O, O, O, O, O, O,
			Y, Y, Y, Y, Y, Y, Y, Y, Y
		}; // z diag

        //p.applyMoves("F U' F2 D' B U R' F' L D' R' U' L U B' D2 R' F U2 D2");
		cout << p.toString() << endl;

		solver.pruningTableMasked.load();
		solver.pruningTableClassic.load();
		solver.redundancyTable.load();
		
		cout << "solver.pruningTableClassic.getStats()";
		cout << "\n--------------------\n";
		cout << solver.pruningTableClassic.getStats();
		cout << "solver.pruningTable.getStats()";
		cout << "\n--------------------\n";
		cout << solver.pruningTableMasked.getStats();
		cout << "\n--------------------\n";
		cout << "solver.redundancyTable.getStats()";
		cout << "\n--------------------\n";
		cout << solver.redundancyTable.getStats();
		cout << "\n--------------------\n";

		auto solutions = solver.solve( p, 14, -1);//16
		cerr << solutions;
		
		auto slnQ = solver.asyncSolveStrings( p, 14, -1);

		try{
			while(slnQ->hasNext()){
				cerr << slnQ->pop() << "\n";
			}
		}catch(runtime_error e){
			cerr << e.what() << endl;
		}

		// string tmp = "";
		// while(*slnQ >> tmp){
		// 	cerr << tmp << "\n";
		// }

		cerr << slnQ->numResults() << "\n";

		//z diag

		//14 -> 1.1 seconds. 
		//15 -> 14.6 seconds
		//16 -> 192 seconds

		//14 -> 1.1 seconds. 
		//15 -> 12.9 seconds
		//16 -> 165 seconds
		

		return 0;
	} catch ( runtime_error& e ) {
		cout << e.what();
		exit( 1 );
	}
}
