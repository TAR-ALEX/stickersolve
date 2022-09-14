#pragma once

#ifndef PUZZLE_TETRAHEDRON_H
#define PUZZLE_TETRAHEDRON_H

#include <stickersolve/solver/puzzle.h>
#include <algorithm>
#include <set>

using namespace std;

Puzzle CreateTetrahedron() {
  Puzzle tetrahedron(
    {
      0, 0, 0, 1,1, 2,2, 3,3, 1,2,3,
      0,1, 0,2, 0,3,
      1,2, 2,3, 3,1
    }
  );
  tetrahedron.addMove(
    "U",
    {
      0,1,2,    3,4,  5,6,  7,8,    10,11,9,

      12,13,   14,15,   16,17,
      20,21,   22,23,   18,19
    }
  );
  tetrahedron.addMove("U'", tetrahedron.getMove("U")*2);


  tetrahedron.addMove(
    "R",
    {
      0,5,2,    3,1,  4,6,  7,8,   9,10,11,

      15,14,   19,18,   16,17,
      12,13,   20,21,   22,23
    }
  );
  tetrahedron.addMove("R'", tetrahedron.getMove("R")*2);



  tetrahedron.addMove(
    "L",
    {
      3,1,2,    8,4,  5,6,  7,0,   9,10,11,

      23,22,   14,15,   13,12,
      18,19,   20,21,   16,17
    }
  );
  tetrahedron.addMove("L'", tetrahedron.getMove("L")*2);

  // tetrahedron.addMove(
  //   "W",
  //   {
  //     0,7,11,   3,2,  6,10,  9,8,   1,5,4,
  //
  //     12,13,   21,20,   16,17,
  //     14,15,   19,18,   22,23
  //   }
  // );
  // tetrahedron.addMove("W'", tetrahedron.getMove("W")*2);

  tetrahedron.addMove(
    "B",
    {
      0,1,7,    3,4,  5,2,  6,8,   9,10,11,

      12,13,   17,16,   21,20,
      18,19,   14,15,   22,23
    }
  );
  tetrahedron.addMove("B'", tetrahedron.getMove("B")*2);


  // tetrahedron.addMove(
  //   "U",
  //   {
  //     0,1,2,    3,4,  5,6,  7,8,   9,10,11,
  //
  //     12,13,   14,15,   16,17,
  //     18,19,   20,21,   22,23
  //   }
  // );
  // tetrahedron.addMove("U'", tetrahedron.getMove("U")*2);

  return tetrahedron;
}

// allowedMovesFromGrip[hand][currentMove][nextAllowed]
vector<vector<vector<pair<string, int>>>> allowedMovesFromGrip =
{
  {//right hand
    {{"R", 1}, {"R'", 2}, {"U", 0}}, // Gripping R 0
    {{"R", 2}, {"R'", 0}, {"B", 1}}, // Gripping R 1
    {{"R", 0}, {"R'", 1}, {"L", 2}}, // Gripping R 2

    {{"U", 4}, {"U'", 5}, {"L", 3}}, // Gripping U 0 //3
    {{"U", 5}, {"U'", 3}, {"B", 4}}, // Gripping U 1 //4
    {{"U", 3}, {"U'", 4}, {"R", 5}}, // Gripping U 2 //5

    {{"L", 7}, {"L'", 8}, {"R", 6}}, // Gripping L //6
    {{"L", 8}, {"L'", 6}, {"B", 7}}, // Gripping L 0 //7
    {{"L", 6}, {"L'", 7}, {"U", 8}}, // Gripping L 0 //8

    {{"B", 10}, {"B'", 11},{"U", 9}}, // Gripping B //9
    {{"B", 11}, {"B'", 9} ,{"L", 10}}, // Gripping B 0 //10
    {{"B", 9},  {"B'", 10},{"R", 11}}, // Gripping B 0 //11
  },
  {//left hand
    {{"R", 1}, {"R'", 2}, {"U'", 0}}, // Gripping R 0
    {{"R", 2}, {"R'", 0}, {"B'", 1}}, // Gripping R 1
    {{"R", 0}, {"R'", 1}, {"L'", 2}}, // Gripping R 2

    {{"U", 4}, {"U'", 5}, {"L'", 3}}, // Gripping U 0 //3
    {{"U", 5}, {"U'", 3}, {"B'", 4}}, // Gripping U 1 //4
    {{"U", 3}, {"U'", 4}, {"R'", 5}}, // Gripping U 2 //5

    {{"L", 7}, {"L'", 8}, {"R'", 6}}, // Gripping L //6
    {{"L", 8}, {"L'", 6}, {"B'", 7}}, // Gripping L 0 //7
    {{"L", 6}, {"L'", 7}, {"U'", 8}}, // Gripping L 0 //8

    {{"B", 10}, {"B'", 11}, {"U'", 9}}, // Gripping B //9
    {{"B", 11}, {"B'", 9},  {"L'", 10}}, // Gripping B 0 //10
    {{"B", 9},  {"B'", 10}, {"R'", 11}}, // Gripping B 0 //11
  }
};

// allowedMovesFromGrip[hand][currentMove][nextAllowed]
// vector<vector<vector<pair<string, int>>>> allowedMovesFromGrip =
// {
//   {//right hand
//     {{"R", 1}, {"R'", 2}}, // Gripping R 0
//     {{"R", 2}, {"R'", 0}}, // Gripping R 1
//     {{"R", 0}, {"R'", 1}}, // Gripping R 2
//
//     {{"U", 4}, {"U'", 5}}, // Gripping U 0 //3
//     {{"U", 5}, {"U'", 3}}, // Gripping U 1 //4
//     {{"U", 3}, {"U'", 4}}, // Gripping U 2 //5
//
//     {{"L", 7}, {"L'", 8}}, // Gripping L //6
//     {{"L", 8}, {"L'", 6}}, // Gripping L 0 //7
//     {{"L", 6}, {"L'", 7}}, // Gripping L 0 //8
//
//     {{"B", 10}, {"B'", 11}}, // Gripping B //9
//     {{"B", 11}, {"B'", 9}}, // Gripping B 0 //10
//     {{"B", 9},  {"B'", 10}}, // Gripping B 0 //11
//   },
//   {//left hand
//     {{"R", 1}, {"R'", 2},}, // Gripping R 0
//     {{"R", 2}, {"R'", 0},}, // Gripping R 1
//     {{"R", 0}, {"R'", 1},}, // Gripping R 2
//
//     {{"U", 4}, {"U'", 5},}, // Gripping U 0 //3
//     {{"U", 5}, {"U'", 3},}, // Gripping U 1 //4
//     {{"U", 3}, {"U'", 4},}, // Gripping U 2 //5
//
//     {{"L", 7}, {"L'", 8},}, // Gripping L //6
//     {{"L", 8}, {"L'", 6},}, // Gripping L 0 //7
//     {{"L", 6}, {"L'", 7},}, // Gripping L 0 //8
//
//     {{"B", 10}, {"B'", 11} }, // Gripping B //9
//     {{"B", 11}, {"B'", 9}  }, // Gripping B 0 //10
//     {{"B", 9},  {"B'", 10} }, // Gripping B 0 //11
//   }
// };

// vector<vector<vector<pair<string, int>>>> allowedMovesFromGrip =
// {
//   {//right hand
//     {{"R", 1}, {"R'", 2}, {"U'", 0}, {"U", 0}}, // Gripping R 0
//     {{"R", 2}, {"R'", 0}, {"B'", 1}, {"B", 1}}, // Gripping R 1
//     {{"R", 0}, {"R'", 1}, {"L'", 2}, {"L", 2}}, // Gripping R 2
//
//     {{"U", 4}, {"U'", 5}, {"L'", 3}, {"L", 3}}, // Gripping U 0 //3
//     {{"U", 5}, {"U'", 3}, {"B'", 4}, {"B", 4}}, // Gripping U 1 //4
//     {{"U", 3}, {"U'", 4}, {"R'", 5}, {"R", 5}}, // Gripping U 2 //5
//
//     {{"L", 7}, {"L'", 8}, {"R'", 6}, {"R", 6}}, // Gripping L //6
//     {{"L", 8}, {"L'", 6}, {"B'", 7}, {"B", 7}}, // Gripping L 0 //7
//     {{"L", 6}, {"L'", 7}, {"U'", 8}, {"U", 8}}, // Gripping L 0 //8
//
//     {{"B", 10}, {"B'", 11}, {"U'", 9}, {"U", 9}}, // Gripping B //9
//     {{"B", 11}, {"B'", 9},  {"L'", 10}, {"L", 10}}, // Gripping B 0 //10
//     {{"B", 9},  {"B'", 10}, {"R'", 11}, {"R", 11}}, // Gripping B 0 //11
//   },
//   {//left hand
//     {{"R", 1}, {"R'", 2}, {"U'", 0}, {"U", 0}}, // Gripping R 0
//     {{"R", 2}, {"R'", 0}, {"B'", 1}, {"B", 1}}, // Gripping R 1
//     {{"R", 0}, {"R'", 1}, {"L'", 2}, {"L", 2}}, // Gripping R 2
//
//     {{"U", 4}, {"U'", 5}, {"L'", 3}, {"L", 3}}, // Gripping U 0 //3
//     {{"U", 5}, {"U'", 3}, {"B'", 4}, {"B", 4}}, // Gripping U 1 //4
//     {{"U", 3}, {"U'", 4}, {"R'", 5}, {"R", 5}}, // Gripping U 2 //5
//
//     {{"L", 7}, {"L'", 8}, {"R'", 6}, {"R", 6}}, // Gripping L //6
//     {{"L", 8}, {"L'", 6}, {"B'", 7}, {"B", 7}}, // Gripping L 0 //7
//     {{"L", 6}, {"L'", 7}, {"U'", 8}, {"U", 8}}, // Gripping L 0 //8
//
//     {{"B", 10}, {"B'", 11}, {"U'", 9}, {"U", 9}}, // Gripping B //9
//     {{"B", 11}, {"B'", 9},  {"L'", 10}, {"L", 10}}, // Gripping B 0 //10
//     {{"B", 9},  {"B'", 10}, {"R'", 11}, {"R", 11}}, // Gripping B 0 //11
//   }
// };

vector<string> impossible = {"impossible"};

vector<string> tryNextMoveWithGrip(vector<string> moves, int l, int r);

vector<string> tryRegrip(vector<string> moves, int l, int r){
//   int originalL = l;
//   int originalR = r;
  vector<string> minRegrips = impossible;
  for(int l = 0; l < allowedMovesFromGrip[1].size(); l++){
    if(l/3 == r/3) continue;
    //if(l/3 == originalL/3 || originalR/3 == r/3) continue;
    vector<string> regrips = tryNextMoveWithGrip(moves, l, r);
    if(regrips == impossible) continue;
    regrips.insert(regrips.begin(), "["+allowedMovesFromGrip[0][l][0].first+" "+allowedMovesFromGrip[0][r][0].first+"]");
    if(minRegrips == impossible || regrips.size() < minRegrips.size())
      minRegrips = regrips;
  }
  for(int r = 0; r < allowedMovesFromGrip[0].size(); r++){
    if(l/3 == r/3) continue;
    //if(l/3 == originalL/3 || originalR/3 == r/3) continue;
    vector<string> regrips = tryNextMoveWithGrip(moves, l, r);
    if(regrips == impossible) continue;
    regrips.insert(regrips.begin(), "["+allowedMovesFromGrip[0][l][0].first+" "+allowedMovesFromGrip[0][r][0].first+"]");
    if(minRegrips == impossible || regrips.size() < minRegrips.size())
      minRegrips = regrips;
  }
  return minRegrips;
}

vector<string> tryNextMoveWithGrip(vector<string> moves, int l, int r){
  if(moves.size() == 0) return {};
  vector<string> regrips = impossible;
  for(int i = 0; i < allowedMovesFromGrip[0][0].size(); i++){
    if(allowedMovesFromGrip[0][r][i].first == moves[0]){
      r = allowedMovesFromGrip[0][r][i].second;
      moves.erase(moves.begin());
      regrips = tryNextMoveWithGrip(moves, l, r);
      if(regrips == impossible) regrips = tryRegrip(moves, l, r);
      return regrips;
    } else if(allowedMovesFromGrip[1][l][i].first == moves[0]){
      l = allowedMovesFromGrip[1][l][i].second;
      moves.erase(moves.begin());
      regrips = tryNextMoveWithGrip(moves, l, r);
      if(regrips == impossible) regrips = tryRegrip(moves, l, r);
      return regrips;
    }
  }
  return regrips;
}

// vector<string> calculateRegrips(vector<string> moves){
//   vector<string> minRegrips = impossible;
//   bool firstPass = true;
//   for(int l = 0; l < allowedMovesFromGrip[1].size(); l++){
//     for(int r = 0; r < allowedMovesFromGrip[0].size(); r++){
//       if(l/3 == r/3) continue;
//       vector<string> regrips = tryNextMoveWithGrip(moves, l, r);
//       if(regrips == impossible) continue;
//       regrips.insert(regrips.begin(), "["+allowedMovesFromGrip[0][l][0].first+" "+allowedMovesFromGrip[0][r][0].first+"]");
//       if(firstPass || regrips < minRegrips) {
//         minRegrips = regrips;
//         firstPass = false;
//       }
//     }
//   }
//   return minRegrips;
// }

vector<string> calculateRegrips(vector<string> moves){

  int l = 8, r = 0;
  vector<string> regrips = tryNextMoveWithGrip(moves, l, r);
  regrips.insert(regrips.begin(), "["+allowedMovesFromGrip[0][l][0].first+" "+allowedMovesFromGrip[0][r][0].first+"]");

  return regrips;
}

void printPyraSolutions(vector<vector<string>> input){
  vector<vector<string>> solutions;
  for(auto solution: input){
    vector<string> solutionString;
    for(int i = 0; i < solution.size(); i++){
      if(
        (solution[i] == "U" || solution[i] == "U'") &&
        (i == solution.size()-1 || i == 0)
      ) continue;
      solutionString.push_back(solution[i]);
    }
    solutions.push_back(solutionString);
  }

  set<vector<string>> s( solutions.begin(), solutions.end() );
  solutions.assign( s.begin(), s.end() );

  sort(
    solutions.begin(),
    solutions.end(),
    [](const vector<string> & a, const vector<string> & b){
      return a.size() < b.size();
    }
  );

  int lastSize = 0;
  for(auto solution: solutions){
    if(solution.size() > lastSize){
      lastSize = solution.size();
      cout << "\nSOLUTIONS OF LENGTH " << lastSize << ": \n";
    }
    for(auto move: solution) cout << move << " ";
    // auto grips = calculateRegrips(solution);
    // for(auto grip: grips) cout << grip << " ";
    cout << endl;
  }
}

Puzzle tetrahedron = CreateTetrahedron();


#endif
