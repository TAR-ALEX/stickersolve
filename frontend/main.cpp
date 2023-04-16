#include <chrono>
#include <iostream>
#include <vector>
using namespace std::chrono;

#include <stickersolve/puzzles/Solver3x3.h>

using namespace std;


#include <algorithm>
#include <iomanip>

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

        // p = {
        //     W, R, W, W, W, W, W, O, W, // classic test, should get all solutions in 1 sec for 14 moves
        //     G, W, G, G, G, G, G, G, G, // 14 -> 1.1 seconds.
        //     R, B, R, R, R, R, R, R, R, // 15 -> 14.6 seconds
        //     B, W, B, B, B, B, B, B, B, // 16 -> 192 seconds
        //     O, G, O, O, O, O, O, O, O, //
        //     Y, Y, Y, Y, Y, Y, Y, Y, Y  // z diag
        // };

        // p.applyMoves("R U2 R2 L2 D L' F' R' L D2 R2 L2 U' L F L'"); // 6 flip
        // p.applyMoves("R D R2 U2 F' L F' L2 U R2 L2 F2 L2 F2 U2 F2 D L2 B' U2");// 1 solution 17
        // p.applyMoves("R U R' U R U R' F' R U R' U' R' F R2 U' R' U2 R U' R'"); // nperm
        p.applyMoves("D' L' D' L U2 R' F2 L2 U' L' D' F D' F' U L U2 R' U'"); // depth 18 done in 70.64 s 232 s laptop
        // cout << p.toString() << endl;
        solver.tableProgressCallback = [&](int p) { solver.cfg->log << "table: " << p << "%\n"; };
        solver.progressCallback = [&](int p) { solver.cfg->log << "solver: " << p << "%\n"; };
        solver.init();


        cout << solver.printTableStats();

        // auto solutions = solver.solve(p, 14, -1); //16
        // cout << solutions;

        auto slnQ = solver.asyncSolveStrings(p, 18, -1);
        // ofstream ff("all_superflip.txt");
        try {
            while (slnQ->hasNext()) { slnQ->pop(); }
            // while (slnQ->hasNext()) { cout << slnQ->pop() << "\n"; }
            // while (slnQ->hasNext()) { auto sol = slnQ->pop(); ff << sol << "\n";cout << sol << "\n";}
        } catch (runtime_error& e) { cout << e.what() << endl; }

        string tmp = "";
        // while (*slnQ >> tmp) { cout << tmp << "\n"; }

        cout << slnQ->numResults() << "\n"; //140

        return 0;
    } catch (runtime_error& e) {
        cout << e.what();
        exit(1);
    }
}
