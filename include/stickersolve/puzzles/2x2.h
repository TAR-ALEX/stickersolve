#pragma once

#ifndef PUZZLE_2x2_H
#define PUZZLE_2x2_H

#include <stickersolve/solver/puzzle.h>

using namespace std;


// Puzzle Create2x2x2() {
//   Puzzle rubiks2(
//     {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5}
//     //{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23}
//   );
//
//   Transformation moveU(
//     "U",
//     {
//       3,  0,  1,  2,  8, 9, 6,  7,  12, 13, 10, 11,
//       16, 17, 14, 15, 4, 5, 18, 19, 20, 21, 22, 23
//     }
//   );
//   Transformation moveR(
//     "R",
//     {
//       0, 5,  6,  3, 4,  21, 22, 7,  11, 8,  9,  10,
//       2, 13, 14, 1, 16, 17, 18, 19, 20, 15, 12, 23
//     }
//   );
//   Transformation moveF(
//     "F",
//     {
//       0,  1,  17, 18, 7,  4,  5,  6,  3,  9, 10, 2,
//       12, 13, 14, 15, 16, 20, 21, 19, 11, 8, 22, 23
//     }
//   );
//   Transformation moveB(
//     "B",
//     {
//       9, 10, 2, 3,
//       4,5,6,7,
//       8,22,23,11,
//       15,12,13,14,
//       1,17,18,0,
//       20,21,19,16
//     }
//   );
//
//   // rubiks2.validMoves.push_back(moveU);
//   // rubiks2.validMoves.push_back(moveU * 2);
//   // rubiks2.validMoves.push_back(moveU * 3);
//   //
//   // rubiks2.validMoves.push_back(moveR);
//   // rubiks2.validMoves.push_back(moveR * 2);
//   // rubiks2.validMoves.push_back(moveR * 3);
//   //
//   // rubiks2.validMoves.push_back(moveF);
//   // rubiks2.validMoves.push_back(moveF * 2);
//   // rubiks2.validMoves.push_back(moveF * 3);
//
//   rubiks2.validMoves.push_back(moveR);
//   rubiks2.validMoves.push_back(moveR * 3);
//   rubiks2.validMoves.push_back(moveU * 3);
//
//   rubiks2.validMoves.push_back(moveR * 2);
//   rubiks2.validMoves.push_back(moveU);
//   rubiks2.validMoves.push_back(moveU * 2);
//
//
//   rubiks2.validMoves.push_back(moveF);
//   rubiks2.validMoves.push_back(moveF * 3);
//
//   rubiks2.validMoves.push_back(moveF * 2);
//
//   // rubiks2.validMoves.push_back(moveB);
//   // rubiks2.validMoves.push_back(moveB * 3);
//   //
//   // rubiks2.validMoves.push_back(moveB * 2);
//
//   return rubiks2;
// }

// Puzzle rubiks2 = Create2x2x2();

#endif
