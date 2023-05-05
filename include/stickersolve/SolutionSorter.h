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
        std::map<int, std::map<std::string, double>> rWeights;
        std::map<int, std::map<std::string, double>> lWeights;

        addMoveswithWeight(rWeights, 0, "U U' D D' R R' L L'", 1.0);
        addMoveswithWeight(rWeights, 0, "R2 L2", 1.8);
        addMoveswithWeight(rWeights, 0, "U2 D2", 1.7);
        addMoveswithWeight(rWeights, 0, "F F'", 1.3);

        addMoveswithWeight(rWeights, 1, "B B' F F' R R' L L'", 1.0);
        addMoveswithWeight(rWeights, 1, "R2 L2", 1.8);
        addMoveswithWeight(rWeights, 1, "B2 F2", 1.7);

        addMoveswithWeight(rWeights, 2, "R R' L L'", 1.0);
        addMoveswithWeight(rWeights, 2, "R2 L2", 1.8);

        addMoveswithWeight(rWeights, 3, "B B' F F' R R' L L'", 1.0);
        addMoveswithWeight(rWeights, 3, "R2 L2", 1.8);
        addMoveswithWeight(rWeights, 3, "B2 F2", 1.7);

        lWeights = rWeights;

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
        pairPenalty("R R2 R'", "L L2 L'", 3);

        pairPenalty("D D2 D'", "F F2 F'", 4);
        pairPenalty("D D2 D'", "B B2 B'", 6);
        pairPenalty("U U2 U'", "F F2 F'", 4);
        pairPenalty("U U2 U'", "B B2 B'", 6);
    }

    double getRatingRegrips(
        std::deque<std::string> solution, std::pair<int, int> grip = {0, 0}, bool forbiddenRegrip = false
    ) { // grip = (left, right)
        // static int ctr = 0;
        // ctr++;
        // if(ctr > 100) exit(0);
        // for (auto& m : solution) std::cout << m << " ";
        // std::cout << std::endl;

        if (solution.empty()) return 0;

        auto move = solution.front();

        if (moveWeights[grip].count(move) == 0) {
            if (forbiddenRegrip) { return std::numeric_limits<double>::max(); }
            //we need regrips
            // return std::numeric_limits<double>::max();
            forbiddenRegrip = true;
            double minRating = std::numeric_limits<double>::max();
            for (int i = 1; i < 4; i++) {
                double newRating = std::numeric_limits<double>::max();
                newRating = getRatingRegrips(solution, {grip.first, (grip.second + i) % 4}, forbiddenRegrip);
                if (newRating <= minRating) minRating = newRating;
                newRating = getRatingRegrips(solution, {(grip.first + i) % 4, grip.second}, forbiddenRegrip);
                if (newRating <= minRating) minRating = newRating;
            }
            return 4.0 + minRating; //regrip is worth 2 moves
        }

        solution.pop_front();

        grip.first += moveGripOffset[move].first;
        grip.second += moveGripOffset[move].second;

        grip.first %= 4;
        grip.second %= 4;

        return moveWeights[grip][move] + getRatingRegrips(solution, grip);
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
            newRating = getRatingRegrips(soldeq, {grip.first, (grip.second + i) % 4}, true);
            if (newRating <= minRating) minRating = newRating;
            newRating = getRatingRegrips(soldeq, {(grip.first + i) % 4, grip.second}, true);
            if (newRating <= minRating) minRating = newRating;
        }
        return minRating + getPairPenalties(soldeq);
    }
};