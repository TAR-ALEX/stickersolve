#include <chrono>
#include <iostream>
#include <vector>
using namespace std::chrono;

#include <stickersolve/puzzles/Solver3x3.h>

using namespace std;

#include <algorithm>
#include <iomanip>

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
    State recolorMask = {
        0,  1,  2,  3,  4,  3,  2,  1,  0,  //
        27, 28, 29, 12, 13, 12, 29, 28, 27, //
        36, 37, 20, 41, 22, 41, 20, 37, 36, //
        27, 28, 29, 12, 31, 12, 29, 28, 27, //
        36, 37, 20, 41, 40, 41, 20, 37, 36, //
        0,  1,  2,  3,  49, 3,  2,  1,  0,  //
    };
    p.solvedState = recolorMask;
    p.state = recolorMask;
    p.generateSymetryTable();
    Puzzle3x3 s1 = p;
    Puzzle3x3 s2 = s1;
    s1.applyMoves("D R");
    s2.applyMoves("R D");
    cout << s1.toString() << endl;
    cout << s2.toString() << endl;
    cout << "s1 == s1 -> " << (s1.state == s2.state) << endl;

    // s1.state = s1.state.recolor(recolorMask);
    // s2.state = s2.state.recolor(recolorMask);

    State trnsfrm = Puzzle3x3().getMove("x2") + Puzzle3x3().getMove("z");
    // s2.state = (s2.state + trnsfrm).recolor(trnsfrm);
    // s2.state = s2.state.recolor(recolorMask);

    s1.state = p.getUniqueSymetric(s1.state);
    s2.state = p.getUniqueSymetric(s2.state);

    cout << trnsfrm.toString() << endl;
    cout << s1.toString() << endl;
    cout << s2.toString() << endl;
    cout << "s1 == s1 -> " << (s1.state == s2.state) << endl;
    return 0;
}