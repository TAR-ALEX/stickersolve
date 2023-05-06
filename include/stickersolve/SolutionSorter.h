#pragma once

#include <deque>
#include <estd/string_util.h>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <vector>

class SolutionSorter {
public:
    SolutionSorter() {}
    virtual double getRating(std::string solution) { return estd::string_util::splitAll(solution, " ", false).size(); }
    std::vector<std::string> sort(std::vector<std::string> input) {
        std::vector<std::string> output;
        std::map<double, std::vector<std::string>> result;
        for (auto& solution : input) {
            auto rating = getRating(solution);
            result[rating].push_back(solution);
        }
        for (auto& [name, vec] : result) {
            for (auto& solution : vec) output.push_back(solution);
        }
        return output;
    }

    std::string sortString(std::vector<std::string> input) {
        std::vector<std::string> output;
        std::map<double, std::vector<std::string>> result;
        for (auto& solution : input) {
            auto rating = getRating(solution);
            result[rating].push_back(solution);
        }
        std::stringstream ss;
        for (auto& [name, vec] : result) {
            ss << "weight: " << name << std::endl;
            ss << "--------------------" << std::endl;
            for (auto& solution : vec) ss << solution << std::endl;
            ss << "--------------------" << std::endl;
        }
        return ss.str();
    }
};

class FingertrickSolutionSorter : public SolutionSorter {
private:
    void initWeights() {
        auto addMoveswithWeight =
            [](std::map<int, std::map<std::string, double>>& weightMap, int grip, std::string movesStr, double weight) {
                auto moves = estd::string_util::splitAll(movesStr, " ", false);
                for (auto& move : moves) { weightMap[grip][move] = weight; }
            };
        auto mirrorMoves = [](std::map<int, std::map<std::string, double>> weightMap) {
            std::map<int, std::map<std::string, double>> weightMapNew;
            for (auto [grip, moves] : weightMap) {
                for (auto [moveConst, weight] : moves) {
                    std::string move = moveConst;
                    move = estd::string_util::replace_all(move, " ", "");
                    move += "'";
                    move = estd::string_util::replace_all(move, "''", "");
                    move = estd::string_util::replace_all(move, "R", "!");
                    move = estd::string_util::replace_all(move, "L", "R");
                    move = estd::string_util::replace_all(move, "!", "L");
                    move = estd::string_util::replace_all(move, "''", "");
                    move = estd::string_util::replace_all(move, "r", "!");
                    move = estd::string_util::replace_all(move, "l", "r");
                    move = estd::string_util::replace_all(move, "!", "l");
                    weightMapNew[grip][move] = weight;
                }
            }
            return weightMapNew;
        };
        std::map<int, std::map<std::string, double>> rWeights;
        std::map<int, std::map<std::string, double>> lWeights;

        addMoveswithWeight(rWeights, 0, "U U' D D' R R' r r'", 1.0);
        addMoveswithWeight(rWeights, 0, "R2 r2", 2.5); // awkward to do in this grip
        addMoveswithWeight(rWeights, 0, "U2 D2", 1.7);
        addMoveswithWeight(rWeights, 0, "F'", 2.5);

        addMoveswithWeight(rWeights, 1, "B B' F F' R' r'", 1.0);
        addMoveswithWeight(rWeights, 1, "R2 r2", 1.8); // awkward to do in this grip
        addMoveswithWeight(rWeights, 1, "R r", 2.5);   // awkward to do in this grip
        addMoveswithWeight(rWeights, 1, "B2 F2", 1.7);

        addMoveswithWeight(rWeights, 2, "R' r'", 1.0);
        addMoveswithWeight(rWeights, 2, "R2 r2", 1.8);
        addMoveswithWeight(rWeights, 2, "R r", 2.5); // is r3

        addMoveswithWeight(rWeights, 3, "B B' F F' R", 1.0);
        addMoveswithWeight(rWeights, 3, "R2 r2", 1.8);
        addMoveswithWeight(rWeights, 3, "R' r'", 2.5);
        addMoveswithWeight(rWeights, 3, "B2 F2", 1.7);

        lWeights = mirrorMoves(rWeights);

        for (auto& [rgrip, rweight] : rWeights) {
            for (auto& [lgrip, lweight] : lWeights) {
                std::pair<int, int> grip = {lgrip, rgrip};
                moveWeights[grip] = lweight;
                for (auto& [move, weight] : rweight) {
                    if (moveWeights[grip].count(move) && moveWeights[grip][move] < weight) continue;
                    moveWeights[grip][move] = weight;
                }
            }
        }
    }

    void initPairPenalties() {
        auto pairPenalty = [&](std::string moves1, std::string moves2, double penalty) {
            auto m1 = estd::string_util::splitAll(moves1, " ", false);
            auto m2 = estd::string_util::splitAll(moves2, " ", false);
            for (auto& l : m1) {
                for (auto& r : m2) {
                    pairPenalties[{l, r}] = penalty;
                    pairPenalties[{r, l}] = penalty;
                }
            }
        };
        pairPenalty("R R2 R' r r2 r'", "L L2 L' l l2 l'", 4);

        pairPenalty("D D2 D'", "F F2 F'", 2);
        pairPenalty("D D2 D'", "B B2 B", 2);
        pairPenalty("U U2 U'", "F F2 F'", 1);
        pairPenalty("U U2 U'", "B B2 B'", 2);
        pairPenalty("F F2 F'", "B B2 B'", 2);
        pairPenalty("U U2 U'", "D D2 D'", 2);
    }

    double getRatingRegrips(
        const std::deque<std::string>& solution,
        const std::pair<int, int>& grip = {0, 0},
        int numRegrips = 2,
        bool lastWasRegrip = true
    ) {
        double regripPenalty = 5;

        if (solution.empty()) return 0;
        // grip = (left, right)
        // static int ctr = 0;
        // ctr++;
        // if (ctr > 100) exit(0);
        // for (auto& m : solution) std::cout << m << " ";
        // std::cout << numRegrips;
        // std::cout << std::endl;



        std::deque<std::string> newSolution = solution;
        std::pair<int, int> newGrip = grip;

        auto move = newSolution.front();
        newSolution.pop_front();

        newGrip.first = (newGrip.first + moveGripOffset[move].first) % 4;
        newGrip.second = (newGrip.second + moveGripOffset[move].second) % 4;

        double minRating = std::numeric_limits<double>::infinity();
        if (moveWeights[newGrip].count(move)) {
            return moveWeights[newGrip][move] + getRatingRegrips(newSolution, newGrip, numRegrips, false);
        }

        if (lastWasRegrip || numRegrips <= 0)
            return regripPenalty * 2 + getRatingRegrips(newSolution, newGrip, numRegrips, false);

        // we can do regrips
        numRegrips--;
        for (int i = 1; i < 4; i++) {
            double newRating = 0;
            
            newGrip = {grip.first, (grip.second + i) % 4};
            if (newGrip.first == 0 || newGrip.second == 0) {
                newRating = regripPenalty + getRatingRegrips(solution, newGrip, numRegrips, true);
                if (newRating <= minRating) minRating = newRating;
            }

            newGrip = {(grip.first + i) % 4, grip.second};
            if (newGrip.first == 0 || newGrip.second == 0) {
                newRating = regripPenalty + getRatingRegrips(solution, newGrip, numRegrips, true);
                if (newRating <= minRating) minRating = newRating;
            }
        }

        return minRating;
    }

    double getPairPenalties(std::deque<std::string> solution) {
        double penalty = 0;
        while (solution.size() >= 2) {
            penalty += pairPenalties[{solution[0], solution[1]}];
            solution.pop_front();
        }
        return penalty;
    }

public:
    FingertrickSolutionSorter() {
        initWeights();
        initPairPenalties();
    }

    std::map<std::pair<std::string, std::string>, double> pairPenalties;

    // map[move] = (left, right)
    std::map<std::string, std::pair<int, int>> moveGripOffset{

        {"R", {0, 1}},  //
        {"R2", {0, 2}}, //
        {"R'", {0, 3}}, //
        {"L'", {1, 0}}, //
        {"L2", {2, 0}}, //
        {"L", {3, 0}},  //
        {"r", {0, 1}},  //
        {"r2", {0, 2}}, //
        {"r'", {0, 3}}, //
        {"l'", {1, 0}}, //
        {"l2", {2, 0}}, //
        {"l", {3, 0}},  //

        // {"LR", {1,0}},//
        // {"LR2", {2,0}},//
        // {"LR'", {3,0}},//
    };

    std::map<std::pair<int, int>, std::map<std::string, double>> moveWeights;


    virtual double getRating(std::string solution) {
        auto moves = estd::string_util::splitAll(solution, " ", false);
        auto soldeq = std::deque<std::string>(moves.begin(), moves.end());
        // return getRatingRegrips(soldeq);

        std::pair<int, int> grip = {0, 0};

        double minRating = std::numeric_limits<double>::max();
        for (int i = 0; i < 4; i++) {
            if (i == 2) continue;
            double newRating = std::numeric_limits<double>::max();
            newRating = getRatingRegrips(soldeq, {grip.first, (grip.second + i) % 4}, 0);
            if (newRating <= minRating) minRating = newRating;
            newRating = getRatingRegrips(soldeq, {(grip.first + i) % 4, grip.second}, 0);
            if (newRating <= minRating) minRating = newRating;
        } //R D' F2 R2 U' L F2 L' U R2 F2 D R' U'
        return minRating + getPairPenalties(soldeq);
    }
};