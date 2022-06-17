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
		Solver3x3 solver("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'   M M2 M' S S2 S' E E2 E'");
		solver.cfg->targetThreads = 4;
		
		Puzzle p = solver.puzzle;
		cout << p.toString() << endl;

		string W = "U";
		string G = "F";
		string R = "R";
		string B = "B";
		string O = "L";
		string Y = "D";
		
		string Q = "?";

		p.state = {
			W, R, W, W, W, W, W, O, W,
			G, W, G, G, G, G, G, G, G,
			R, B, R, R, R, R, R, R, R,
			B, W, B, B, B, B, B, B, B,
			O, G, O, O, O, O, O, O, O,
			Y, Y, Y, Y, Y, Y, Y, Y, Y
		}; // z diag

        //p.applyMoves("F U' F2 D' B U R' F' L D' R' U' L U B' D2 R' F U2 D2");
		cout << p.toString() << endl;

		solver.pruningTable.load();
		solver.redundancyTable.load();
		
		cout << "solver.pruningTable.getStats()";
		cout << "\n--------------------\n";
		cout << solver.pruningTable.getStats();
		cout << "\n--------------------\n";
		cout << "solver.redundancyTable.getStats()";
		cout << "\n--------------------\n";
		cout << solver.redundancyTable.getStats();
		cout << "\n--------------------\n";

		auto solutions = solver.solve( p, 14, 1);//16
		cerr << solutions;

		return 0;
	} catch ( runtime_error& e ) {
		cout << e.what();
		exit( 1 );
	}
}
