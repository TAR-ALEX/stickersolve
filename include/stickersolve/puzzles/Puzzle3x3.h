#pragma once

#include <regex>
#include <stickersolve/solver/PuzzleSolver.h>
#include <boost/container/static_vector.hpp>

using namespace std;


class Puzzle3x3 : public Puzzle {
public:
    Puzzle3x3() :
        Puzzle{
            {
                0, 0, 0, 0, 0, 0, 0, 0, 0, //
                1, 1, 1, 1, 1, 1, 1, 1, 1, //
                2, 2, 2, 2, 2, 2, 2, 2, 2, //
                3, 3, 3, 3, 3, 3, 3, 3, 3, //
                4, 4, 4, 4, 4, 4, 4, 4, 4, //
                5, 5, 5, 5, 5, 5, 5, 5, 5, //
            },
        } {
        // ---------------------------------------------------
        //        Move that does nothing
        // ---------------------------------------------------
        // State move = {
        //     0,  1,  2,  3,  4,  5,  6,  7,  8,  //
        //     9,  10, 11, 12, 13, 14, 15, 16, 17, //
        //     18, 19, 20, 21, 22, 23, 24, 25, 26, //
        //     27, 28, 29, 30, 31, 32, 33, 34, 35, //
        //     36, 37, 38, 39, 40, 41, 42, 43, 44, //
        //     45, 46, 47, 48, 49, 50, 51, 52, 53  //
        // };

        {
            State move = {
                6,  3,  0,  7,  4,  1,  8,  5,  2,  //
                18, 19, 20, 12, 13, 14, 15, 16, 17, //
                27, 28, 29, 21, 22, 23, 24, 25, 26, //
                36, 37, 38, 30, 31, 32, 33, 34, 35, //
                9,  10, 11, 39, 40, 41, 42, 43, 44, //
                45, 46, 47, 48, 49, 50, 51, 52, 53  //
            };
            this->addMove("U", move);
            this->addMove("U2", move * 2);
            this->addMove("U'", move * 3);
        }
        {
            State move = {
                0,  1,  11, 3,  4,  14, 6,  7,  17, //
                9,  10, 47, 12, 13, 50, 15, 16, 53, //
                24, 21, 18, 25, 22, 19, 26, 23, 20, //
                8,  28, 29, 5,  31, 32, 2,  34, 35, //
                36, 37, 38, 39, 40, 41, 42, 43, 44, //
                45, 46, 33, 48, 49, 30, 51, 52, 27  //
            };
            this->addMove("R", move);
            this->addMove("R2", move * 2);
            this->addMove("R'", move * 3);
        }
        {
            State move = {
                0,  1,  2,  3,  4,  5,  44, 41, 38, //
                15, 12, 9,  16, 13, 10, 17, 14, 11, //
                6,  19, 20, 7,  22, 23, 8,  25, 26, //
                27, 28, 29, 30, 31, 32, 33, 34, 35, //
                36, 37, 45, 39, 40, 46, 42, 43, 47, //
                24, 21, 18, 48, 49, 50, 51, 52, 53  //
            };
            this->addMove("F", move);
            this->addMove("F2", move * 2);
            this->addMove("F'", move * 3);
        }
        {
            State move = {
                35, 1,  2,  32, 4,  5,  29, 7,  8,  //
                0,  10, 11, 3,  13, 14, 6,  16, 17, //
                18, 19, 20, 21, 22, 23, 24, 25, 26, //
                27, 28, 51, 30, 31, 48, 33, 34, 45, //

                42, 39, 36, 43, 40, 37, 44, 41, 38, //
                9,  46, 47, 12, 49, 50, 15, 52, 53  //
            };
            this->addMove("L", move);
            this->addMove("L2", move * 2);
            this->addMove("L'", move * 3);
        }
        {
            State move = {
                0,  1,  2,  3,  4,  5,  6,  7,  8, //

                9,  10, 11, 12, 13, 14, 42, 43, 44, //
                18, 19, 20, 21, 22, 23, 15, 16, 17, //
                27, 28, 29, 30, 31, 32, 24, 25, 26, //
                36, 37, 38, 39, 40, 41, 33, 34, 35, //

                51, 48, 45, 52, 49, 46, 53, 50, 47 //
            };
            this->addMove("D", move);
            this->addMove("D2", move * 2);
            this->addMove("D'", move * 3);
        }
        {
            State move = {
                20, 23, 26, 3,  4,  5,  6,  7,  8,  //
                9,  10, 11, 12, 13, 14, 15, 16, 17, //
                18, 19, 53, 21, 22, 52, 24, 25, 51, //
                33, 30, 27, 34, 31, 28, 35, 32, 29, //
                2,  37, 38, 1,  40, 41, 0,  43, 44, //
                45, 46, 47, 48, 49, 50, 36, 39, 42  //
            };
            this->addMove("B", move);
            this->addMove("B2", move * 2);
            this->addMove("B'", move * 3);
        }
        {
            State move = {
                0,  10, 2,  3,  13, 5,  6,  16, 8,  //
                9,  46, 11, 12, 49, 14, 15, 52, 17, //
                18, 19, 20, 21, 22, 23, 24, 25, 26, //
                27, 7,  29, 30, 4,  32, 33, 1,  35, //
                36, 37, 38, 39, 40, 41, 42, 43, 44, //
                45, 34, 47, 48, 31, 50, 51, 28, 53  //
            };
            this->addMove("M'", move);
            this->addMove("M2", move * 2);
            this->addMove("M", move * 3);
        }
        {
            State move = {
                0,  1,  2,  43, 40, 37, 6,  7,  8,  //
                9,  10, 11, 12, 13, 14, 15, 16, 17, //
                18, 3,  20, 21, 4,  23, 24, 5,  26, //
                27, 28, 29, 30, 31, 32, 33, 34, 35, //
                36, 48, 38, 39, 49, 41, 42, 50, 44, //
                45, 46, 47, 25, 22, 19, 51, 52, 53  //
            };
            this->addMove("S", move);
            this->addMove("S2", move * 2);
            this->addMove("S'", move * 3);
        }
        {
            State move = {
                0,  1,  2,  3,  4,  5,  6,  7,  8,  //
                9,  10, 11, 21, 22, 23, 15, 16, 17, //
                18, 19, 20, 30, 31, 32, 24, 25, 26, //
                27, 28, 29, 39, 40, 41, 33, 34, 35, //
                36, 37, 38, 12, 13, 14, 42, 43, 44, //
                45, 46, 47, 48, 49, 50, 51, 52, 53  //
            };
            this->addMove("E'", move);
            this->addMove("E2", move * 2);
            this->addMove("E", move * 3);
        }
        {
            State move = {
                0,  10, 11, 3,  13, 14, 6,  16, 17, //
                9,  46, 47, 12, 49, 50, 15, 52, 53, //
                24, 21, 18, 25, 22, 19, 26, 23, 20, //
                8,  7,  29, 5,  4,  32, 2,  1,  35, //
                36, 37, 38, 39, 40, 41, 42, 43, 44, //
                45, 34, 33, 48, 31, 30, 51, 28, 27  //
            };

            this->addMove("r", move);
            this->addMove("r2", move * 2);
            this->addMove("r'", move * 3);
        }
        {
            State move = {
                0,  1,  2,  43, 40, 37, 44, 41, 38, //
                15, 12, 9,  16, 13, 10, 17, 14, 11, //
                6,  3,  20, 7,  4,  23, 8,  5,  26, //
                27, 28, 29, 30, 31, 32, 33, 34, 35, //
                36, 48, 45, 39, 49, 46, 42, 50, 47, //
                24, 21, 18, 25, 22, 19, 51, 52, 53  //
            };
            this->addMove("f", move);
            this->addMove("f2", move * 2);
            this->addMove("f'", move * 3);
        }
        {
            State move = {
                6,  3,  0,  7,  4,  1,  8,  5,  2,  //
                18, 19, 20, 21, 22, 23, 15, 16, 17, //
                27, 28, 29, 30, 31, 32, 24, 25, 26, //
                36, 37, 38, 39, 40, 41, 33, 34, 35, //
                9,  10, 11, 12, 13, 14, 42, 43, 44, //
                45, 46, 47, 48, 49, 50, 51, 52, 53  //
            };
            this->addMove("u", move);
            this->addMove("u2", move * 2);
            this->addMove("u'", move * 3);
        }

        {
            State move = this->getMove("B");
            move += this->getMove("S'");
            this->addMove("b", move);
            this->addMove("b2", move * 2);
            this->addMove("b'", move * 3);
        }
        {
            State move = this->getMove("D");
            move += this->getMove("E");
            this->addMove("d", move);
            this->addMove("d2", move * 2);
            this->addMove("d'", move * 3);
        }
        {
            State move = this->getMove("L");
            move += this->getMove("M");
            this->addMove("l", move);
            this->addMove("l2", move * 2);
            this->addMove("l'", move * 3);
        }

        {
            State move = this->getMove("R");
            move += this->getMove("M'");
            move += this->getMove("L'");
            this->addMove("x", move);
            this->addMove("x2", move * 2);
            this->addMove("x'", move * 3);
        }
        {
            State move = this->getMove("F");
            move += this->getMove("S");
            move += this->getMove("B'");
            this->addMove("z", move);
            this->addMove("z2", move * 2);
            this->addMove("z'", move * 3);
        }
        {
            State move = this->getMove("U");
            move += this->getMove("E'");
            move += this->getMove("D'");
            this->addMove("y", move);
            this->addMove("y2", move * 2);
            this->addMove("y'", move * 3);
        }
        {
            State move = this->getMove("U") + this->getMove("D'");
            this->addMove("UD", move);
            this->addMove("UD2", move * 2);
            this->addMove("UD'", move * 3);
        }
        {
            State move = this->getMove("R") + this->getMove("L'");
            this->addMove("RL", move);
            this->addMove("RL2", move * 2);
            this->addMove("RL'", move * 3);
        }
        {
            State move = this->getMove("F") + this->getMove("B'");
            this->addMove("FB", move);
            this->addMove("FB2", move * 2);
            this->addMove("FB'", move * 3);
        }
    }

    Puzzle3x3(string allowed) : Puzzle3x3() { keepOnlyMoves(allowed); }

    Puzzle3x3 getPiecePuzzle() {
        Puzzle3x3 result = *this;
        result.solvedState = getPieceState(solvedState);
        result.state = getPieceState(state);
        result.generateSymetryTable();
        return result;
    }
public:
    clonable* clone() const{
        return new Puzzle3x3(*this);
    }
    boost::container::static_vector<std::pair<State, State>, 48> symetryTable = {};
    //std::vector<std::pair<State, State>> symetryTable = {};// transform, recolor
    std::unordered_map<uint16_t, State> standardOrientationTable = {};
    void generateSymetryTable() {
        static State S_URF3 = Puzzle3x3().getMove("x") + Puzzle3x3().getMove("y");
        static State S_F2 = Puzzle3x3().getMove("z2"); // check axis 4 times
        static State S_U4 = Puzzle3x3().getMove("y");  // do a z

        std::vector<State> rotationTables;
        std::vector<State> stateTables;
        State symetry;

        State S_MIRROR_L_R = {
            2,  1,  0,  5,  4,  3,  8,  7,  6,  //
            11, 10, 9,  14, 13, 12, 17, 16, 15, //
            38, 37, 36, 41, 40, 39, 44, 43, 42, //
            29, 28, 27, 32, 31, 30, 35, 34, 33, //
            20, 19, 18, 23, 22, 21, 26, 25, 24, //
            47, 46, 45, 50, 49, 48, 53, 52, 51, //
        };


        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 4; k++) {
                    for (int m = 0; m < 2; m++) {
                        rotationTables.push_back((S_URF3 * i) + (S_F2 * j) + (S_U4 * k) + (S_MIRROR_L_R * m));
                        State rr = solvedState + rotationTables.back();
                        stateTables.push_back(rr);
                        if (m == 0) standardOrientationTable[55 * rr[4] + rr[13]] = !rotationTables.back();
                        // if (checkDuplicateStates()) {
                        //     rotationTables.pop_back();
                        //     stateTables.pop_back();
                        // }
                    }
                }
            }
        }

        auto genRecolor = [&](State from, State to) {
            State recolorMask;

            for (size_t i = 0; i < from.size(); i++) {
                if ((int)recolorMask.size() < from[i] + 1) recolorMask.resize(from[i] + 1, -1);
                if (from[i] < 0) throw std::runtime_error("negative values");
                if (recolorMask[from[i]] != -1 && recolorMask[from[i]] != to[i])
                    throw std::runtime_error(
                        "conflicting values for recolor from[" + std::to_string(from[i]) +
                        "] = " + std::to_string(recolorMask[from[i]]) + ", " + "to[" + std::to_string(from[i]) +
                        "] = " + std::to_string(to[i])
                    );
                recolorMask[from[i]] = to[i];
            }
            return recolorMask;
        };
        symetryTable.clear();
        for (size_t i = 0; i < stateTables.size(); i++) {
            try {
                symetryTable.push_back({rotationTables[i], genRecolor(stateTables[i], solvedState)});
                // std::cout << "sym: " << i <<std::endl;
            } catch (std::runtime_error& e) {
                // std::cout << e.what() <<std::endl;
            }
        }
    }

public:
    State getStandardOrientation(const State& s) {
        return s + standardOrientationTable[55 * s[4] + s[13]];
    }
    State getUniqueSymetricOld(const State& s) {
        State min = s;
        int minVal = INT_MAX;
        State tst;
        boost::container::static_vector<size_t, 100> continueSearch = {};
        // continueSearch.reserve(symetryTable.size() / 2);
        // for (size_t i = 0; i < symetryTable.size(); i++) {
        //     auto& [t, r] = symetryTable[i];
        //     tst = s.recolor(r)+t;
        //     if (tst < min) min = tst;
        // }


        for (size_t j = 0; j < symetryTable.size(); j++) {
            auto& [t, r] = symetryTable[j];
            int sticker = r[s[t[0]]];
            if (sticker < minVal) {
                minVal = sticker;
                continueSearch.clear();
                continueSearch.push_back(j);
            } else if (sticker == minVal) {
                continueSearch.push_back(j);
            }
        }

        boost::container::static_vector<size_t, 100> continueSearchCpy;
        for (size_t i = 1; i < s.size(); i++) {
            continueSearchCpy.clear();
            // continueSearchCpy.reserve(symetryTable.size() / 2);
            std::swap(continueSearchCpy, continueSearch);
            minVal = INT_MAX;
            for (auto& srch : continueSearchCpy) {
                auto& [t, r] = symetryTable[srch];
                int sticker = r[s[t[i]]];
                if (sticker < minVal) {
                    minVal = sticker;
                    continueSearch.clear();
                    continueSearch.push_back(srch);
                } else if (sticker == minVal) {
                    continueSearch.push_back(srch);
                }
            }

            if (continueSearch.size() == 1) {
                auto& [t, r] = symetryTable[continueSearch[0]];
                return s.recolor(r) + t;
            }
        }


        auto& [t, r] = symetryTable[continueSearch[0]];
        return s.recolor(r) + t;
    }

    //new version, faster?
    State getUniqueSymetric(const State& s) {
        int minVal = INT_MAX;

        const size_t SEARCH_SIZE = 48;
        int continueSearch[SEARCH_SIZE] = {};
        size_t curSize = 0;
        //for (size_t j = 0; j < symetryTable.size(); j++) continueSearch[j] = j;

        for (size_t j = 0; j < symetryTable.size(); j++) {
            auto& [t, r] = symetryTable[j];
            int sticker = r[s[t[0]]];
            if (sticker < minVal) {
                minVal = sticker;
                curSize = 0;
                continueSearch[curSize] = j;
                curSize++;
            } else if (sticker == minVal) {
                continueSearch[curSize] = j;
                curSize++;
            }
        }

        int atIndex = 0;
//if (curSize != 1) 
        for (size_t i = 1; i < s.size(); i++) {
            minVal = INT_MAX;
            atIndex = 0;
            for (size_t j = 0; j < curSize; j++) {
                auto& [t, r] = symetryTable[continueSearch[j]];
                int sticker = r[s[t[i]]];
                if (sticker < minVal) {
                    minVal = sticker;
                    atIndex = 0;
                    continueSearch[atIndex] = continueSearch[j];
                    atIndex++;
                } else if (sticker == minVal) {
                    continueSearch[atIndex] = continueSearch[j];
                    atIndex++;
                }
            }
            curSize = atIndex;
            if (curSize == 1) {
                break;
            }
        }

        auto& [t, r] = symetryTable[continueSearch[0]];
        return s.recolor(r) + t;

    }

    State getUniqueSymetricInverse(const State& s) {
        auto s1 = getUniqueSymetric(s);
        auto s2 = getUniqueSymetric(!s);
        if(s2 < s1) s1 = s2;
        return s1;

        int minVal = INT_MAX;

        const State sinv = !s;

        const size_t SEARCH_SIZE = 48*2;
        int continueSearch[SEARCH_SIZE] = {};
        size_t curSize = 0;

        for (size_t j = 0; j < symetryTable.size()*2; j++) {
            auto& [t, r] = symetryTable[j%symetryTable.size()];
            int sticker;
            if(j < symetryTable.size()){
                sticker = r[s[t[0]]];
            } else {
                sticker = r[sinv[t[0]]];
            }
            if (sticker < minVal) {
                minVal = sticker;
                curSize = 0;
                continueSearch[curSize] = j;
                curSize++;
            } else if (sticker == minVal) {
                continueSearch[curSize] = j;
                curSize++;
            }
        }

        int atIndex = 0;

        for (size_t i = 1; i < s.size(); i++) {
            minVal = INT_MAX;
            atIndex = 0;
            for (size_t j = 0; j < curSize; j++) {
                auto& [t, r] = symetryTable[continueSearch[j]%symetryTable.size()];
                int sticker;
                if(continueSearch[j] < (int)symetryTable.size()){
                    sticker = r[s[t[i]]];
                } else {
                    sticker = r[sinv[t[i]]];
                }
                if (sticker < minVal) {
                    minVal = sticker;
                    atIndex = 0;
                    continueSearch[atIndex] = continueSearch[j];
                    atIndex++;
                } else if (sticker == minVal) {
                    continueSearch[atIndex] = continueSearch[j];
                    atIndex++;
                }
            }
            curSize = atIndex;
            if (curSize == 1) {
                break;
            }
        }

        if(continueSearch[0] < (int)symetryTable.size()){
            auto& [t, r] = symetryTable[continueSearch[0]];
            return s.recolor(r) + t;
        } else {
            auto& [t, r] = symetryTable[continueSearch[0]%symetryTable.size()];
            return sinv.recolor(r) + t;
        }
    }
};