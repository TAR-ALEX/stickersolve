#include <chrono>
#include <iostream>
#include <vector>
using namespace std::chrono;

#include <stickersolve/puzzles/Solver3x3.h>
#include <stickersolve/solver/puzzle.h>

using namespace std;


#include <algorithm>
#include <iomanip>
#include <stickersolve/solver/PuzzleSolver.h>

int main() {
    cerr << "----------------------------------------------------------------\n";
    try {
        Solver3x3 solver("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'");

        solver.cfg->pruiningTablesPath = "./tables";

        Puzzle p = Puzzle3x3("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'");

        // cout << p.toString() << endl;

        int W = 0;
        int G = 1;
        int R = 2;
        int B = 3;
        int O = 4;
        int Y = 5;

        int Q = 6;

        // p.applyMoves("U R2 F B R B2 R U2 L B2 R U' D' R2 F R' L B2 U2 F2"); // superflip finds 1 solution in 34 seconds
        // p.applyMoves("F U' F2 D' B U R' F' L D' R' U' L U B' D2 R' F U2 D2");// hardest

        p = {
            W, R, W, W, W, W, W, O, W, // classic test, should get all solutions in 1 sec for 14 moves
            G, W, G, G, G, G, G, G, G, // 14 -> 1.1 seconds.
            R, B, R, R, R, R, R, R, R, // 15 -> 14.6 seconds
            B, W, B, B, B, B, B, B, B, // 16 -> 192 seconds
            O, G, O, O, O, O, O, O, O, //
            Y, Y, Y, Y, Y, Y, Y, Y, Y  // z diag
        };

        //p.applyMoves("F U' F2 D' B U R' F' L D' R' U' L U B' D2 R' F U2 D2");
        // p.applyMoves("U2 L2 R2 B2 R2 D2 B' D2 R2 B D2 L' F2 D' L' F L' D' L");// new test
        // cout << p.toString() << endl;

        solver.init();
        cout << solver.printTableStats();

        // auto solutions = solver.solve(p, 14, -1); //16
        // cout << solutions;

        auto slnQ = solver.asyncSolveStrings(p, 14, -1);
        try {
            // while (slnQ->hasNext()) { slnQ->pop(); }
            while (slnQ->hasNext()) { cout << slnQ->pop() << "\n"; }
        } catch (runtime_error& e) { cout << e.what() << endl; }

        string tmp = "";
        // while (*slnQ >> tmp) { cout << tmp << "\n"; }

        cout << slnQ->numResults() << "\n"; //140

        //z diag

        //14 -> 1.1 seconds.
        //15 -> 14.6 seconds
        //16 -> 192 seconds

        //14 -> 1.1 seconds.
        //15 -> 12.9 seconds
        //16 -> 165 seconds

        //14.3 sec
        return 0;
    } catch (runtime_error& e) {
        cout << e.what();
        exit(1);
    }
}
