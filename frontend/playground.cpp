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

int main2() {
    Puzzle3x3 p("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'");

    auto r = p.getStickerGroups();
    for (auto& grp : r) {
        for (auto& piece : grp) {
            cout << "[";
            for (auto& sticker : piece) { cout << sticker << ' '; }
            cout << "\e[D] ";
        }
        cout << endl;
    }

    auto m = p.getStickerMap();
    for (auto [k, v] : m) {
        cout << "[" << k.first << ": ";
        for (auto c : k.second) cout << c << ",";
        cout << "\e[D] = [";
        for (auto sId : v) cout << sId << " ";
        cout << "\e[D] " << endl;
    }

    auto pp = p.getPiecePuzzle();
    cout << pp.toString() << endl;
    return 0;
}

int main() {
    Puzzle3x3 p("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'");
    State puzzleMask = {
        0,  1,  2,  3,  -1,  3,  2,  1,  0,  //
        -1, -1, -1, 12, -1, 12, -1, -1, -1, //
        -1, -1, -1, -1, -1, -1, -1, -1, -1, //
        -1, -1, -1, 12, -1, 12, -1, -1, -1, //
        -1, -1, -1, -1, -1, -1, -1, -1, -1, //
        0,  1,  2,  3,  -1, 3,  2,  1,  0,  //
    };
    Puzzle3x3 s1 = p.getPiecePuzzle();
    Puzzle3x3 s2 = s1;
    s1.applyMoves("D R");
    s2.applyMoves("R D");
    cout << s1.toString() << endl;
    cout << s2.toString() << endl;
    cout << "s1 == s1 -> " << (s1.state == s2.state) << endl;


    s1.state = p.getUniqueSymetric(s1).recolor(puzzleMask);
    s2.state = p.getUniqueSymetric(s2).recolor(puzzleMask);

    cout << s1.toString() << endl;
    cout << s2.toString() << endl;
    cout << "s1 == s1 -> " << (s1.state == s2.state) << endl;
    return 0;
}