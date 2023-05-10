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
        // Solver3x3 solver("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'");
        Solver3x3 solver;
        solver.cfg->pruiningTablesPath = "./tables";

        Puzzle p = Puzzle3x3("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B' M M2 M'"); //M M2 M' S S2 S' E E2 E'

        p.applyMoves("R U R' U' R' F R2 U' R' U' R U R' F'"); // t perm
        solver.tableProgressCallback = [&](int p) { solver.cfg->log << "table: " << p << "%\n"; };
        // solver.progressCallback = [&](int p) { solver.cfg->log << "solver: " << p << "%\n"; };
        solver.init();

        // cout << solver.printTableStats();

        auto slnQ = solver.asyncSolveStrings(p, 14, -1);

        string tmp = "";
        std::vector<std::string> solutions;
        while (*slnQ >> tmp) {
            solutions.push_back(tmp);
            //cout << tmp << "\n";
        }

        cout << slnQ->numResults() << "\n"; //140

        std::cout << "\n\n------------------\n\n";

        solutions = FingertrickSolutionSorter().sort(solutions);


        for(auto& sol: solutions) std::cout << sol << std::endl;
        return 0;
    } catch (runtime_error& e) {
        cout << e.what();
        exit(1);
    }
}
