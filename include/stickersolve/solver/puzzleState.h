#pragma once

#include <initializer_list>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

class State : public vector<int> {
public:
    using vector<int>::vector;
    friend State operator!(State t);
    friend State operator*(State left, int right);
    friend State operator+(State left, State right);
    friend State operator-(State left, State right);
    friend State& operator+=(State& left, const State& right);
    friend State& operator-=(State& left, const State& right);
    State recolor(const vector<int>& colorMap) const {
        State result(size());
        for (size_t i = 0; i < size(); i++) {
            result[i] = colorMap[(*this)[i]];
        }
        return result;
    }
    void inverseRecolor(const vector<int>& colorMapInv) {
        vector<int> colorMap;
        colorMap.resize(colorMapInv.size());
        for (size_t i = 0; i < colorMapInv.size(); i++) { colorMap[colorMapInv[i]] = i; }
        *this = recolor(colorMap);
    }
    bool isNOP();
    State applyMask(State m) {
        State delta(size());
        for (size_t i = 0; i < size(); i++) {
            if (m[i] != 0) delta[i] = (*this)[i];
        }
        return delta;
    }
    //  	State( vector<int> ints ): vector<int>(ints) {};
    //  	State() : vector<int>() {};
    State& operator=(vector<string> t) {
        *this = State(t);
        return *this;
    };
    State& operator=(initializer_list<string> lst) {
        *this = State(lst);
        return *this;
    };
    State(initializer_list<string> lst) : State(vector<string>(lst)){};
    State(vector<string> strs) {
        map<string, int> mapping;

        for (auto e : strs) mapping.insert({e, 0});
        int i = 0;

        for (auto& e : mapping) e.second = i++;

        for (auto e : strs) push_back(mapping[e]);
    }
    string toString();
    uint64_t toHash() const;
};
