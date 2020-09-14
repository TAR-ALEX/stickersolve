#include <iostream>
#include <vector>
#include <chrono>
using namespace std::chrono;

#include "puzzle.h"
#include "puzzles/tertahedron.h"
#include "puzzles/Solver3x3.h"
#include "puzzles/Solver3x3_CLL.h"
#include "puzzles/Solver3x3_CLL_QTM.h"
#include "puzzles/FingertricksSolver3x3.h"

using namespace std;


#include <iomanip>
#include "PuzzleSolver.h"
#include <algorithm>
		
int main( int argc, char const* argv[] ) {
	cerr << "----------------------------------------------------------------\n";
	try {
// 		FingertricksSolver3x3 solver;
		//Solver3x3Cll solver;
		Solver3x3Cll solver;
		//Solver3x3 solver;
		
		State initial = {
			4, 0, 3, 0, 0, 0, 0, 0, 2,
			1, 3, 0, 1, 1, 1, 1, 1, 1,
			1, 1, 0, 2, 2, 2, 2, 2, 2,
			2, 2, 0, 3, 3, 3, 3, 3, 3,
			3, 4, 4, 4, 4, 4, 4, 4, 4,
			5, 5, 5, 5, 5, 5, 5, 5, 5
		}; // sune

		initial = {
			0, 0, 0, 1, 0, 3, 0, 0, 0,
			1, 4, 1, 1, 1, 1, 1, 1, 1,
			2, 0, 2, 2, 2, 2, 2, 2, 2,
			3, 2, 3, 3, 3, 3, 3, 3, 3,
			4, 0, 4, 4, 4, 4, 4, 4, 4,
			5, 5, 5, 5, 5, 5, 5, 5, 5
		}; // z diag
		string W = "U";
		string G = "F";
		string R = "R";
		string B = "B";
		string O = "L";
		string Y = "D";
		
		string Q = "?";

		initial = {
			W, W, W, G, W, B, W, W, W,
			G, O, G, G, G, G, G, G, G,
			R, W, R, R, R, R, R, R, R,
			B, R, B, B, B, B, B, B, B,
			O, W, O, O, O, O, O, O, O,
			Y, Y, Y, Y, Y, Y, Y, Y, Y
		}; // z diag
		
		initial = {
			W, W, W, O, W, R, W, W, W,
			G, G, G, G, G, G, G, G, G,
			R, W, R, R, R, R, R, R, R,
			B, B, B, B, B, B, B, B, B,
			O, W, O, O, O, O, O, O, O,
			Y, Y, Y, Y, Y, Y, Y, Y, Y
		}; // 2 flip opp
		
// 		initial = {
// 			W, B, W, O, W, R, W, G, W,
// 			G, W, G, G, G, G, G, G, G,
// 			R, W, R, R, R, R, R, R, R,
// 			B, W, B, B, B, B, B, B, B,
// 			O, W, O, O, O, O, O, O, O,
// 			Y, Y, Y, Y, Y, Y, Y, Y, Y
// 		}; // 4 flip opp
		
// 		initial = {
// 			W, W, W, W, W, G, W, R, W,
// 			G, W, G, G, G, G, G, G, G,
// 			R, W, R, R, R, R, R, R, R,
// 			B, O, B, B, B, B, B, B, B,
// 			O, B, O, O, O, O, O, O, O,
// 			Y, Y, Y, Y, Y, Y, Y, Y, Y
// 		}; // 2 z adj good
		
// 		initial = {
// 			W, R, W, G, W, W, W, W, W,
// 			G, O, G, G, G, G, G, G, G,
// 			R, B, R, R, R, R, R, R, R,
// 			B, W, B, B, B, B, B, B, B,
// 			O, W, O, O, O, O, O, O, O,
// 			Y, Y, Y, Y, Y, Y, Y, Y, Y
// 		}; // 2 z adj bad
		
		
// 		initial = {
// 			W, W, W, W, W, O, W, B, W,
// 			G, W, G, G, G, G, G, G, G,
// 			R, W, R, R, R, R, R, R, R,
// 			B, G, B, B, B, B, B, B, B,
// 			O, R, O, O, O, O, O, O, O,
// 			Y, Y, Y, Y, Y, Y, Y, Y, Y
// 		}; // 2 h adj 

		initial = {
			W, O, W, B, W, G, W, R, W,
			G, W, G, G, G, G, G, G, G,
			R, W, R, R, R, R, R, R, R,
			B, W, B, B, B, B, B, B, B,
			O, W, O, O, O, O, O, O, O,
			Y, Y, Y, Y, Y, Y, Y, Y, Y
		}; // 4 FLIP Z
		
		initial = {
			R, Q, O, Q, W, Q, O, Q, R,
			B, Q, B, G, G, G, G, G, G,
			W, Q, G, R, R, R, R, R, R,
			W, Q, W, B, B, B, B, B, B,
			G, Q, W, O, O, O, O, O, O,
			Y, Y, Y, Y, Y, Y, Y, Y, Y
		}; // checkers pi
		
		initial = {
			R, Q, O, Q, W, Q, W, Q, W,
			O, Q, R, G, G, G, G, G, G,
			B, Q, G, R, R, R, R, R, R,
			W, Q, W, B, B, B, B, B, B,
			G, Q, B, O, O, O, O, O, O,
			Y, Y, Y, Y, Y, Y, Y, Y, Y
		}; // checkers headlights
		
// 		cout << rubiks3.pruningTable.getStats();
// 		cout << "\n--------------------\n";
// 		cout << rubiks3.redundancyTable.toString();
// 		cout << "\n--------------------\n";
		solver.defaultGrip = 0;
		auto solutions = solver.solve( initial, 16 );//16
		cerr << solutions;

		return 0;
	} catch ( runtime_error& e ) {
		cout << e.what();
		exit( 1 );
	}
}
