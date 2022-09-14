#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <map>

#include <stickersolve/solver/puzzleState.h>

using namespace std;

string State::toString() {
  string result = "[";
  for (int i = 0; i < size(); i++) {
    if (i != 0) {
      result += ", ";
    }
    result += to_string((*this)[i]);
  }
  return result + "]";
}

State operator+(State left, State right) {
  State delta = left;
  for (int i = 0; i < left.size(); i++) {
    delta[i] = left[right[i]];
  }
  return delta;
}

State operator*(State left, int right) {
  auto result = left;
  for (int i = 1; i < right; i++) {
    result += left;
  }
  return result;
}

State operator-( State left, State right ) {
	State delta = left;

	for ( int i = 0; i < left.size(); i++ ) {
		delta[right[i]] = left[i];
	}

	return delta;
	return left + ( !right );
}

State& operator +=(State& left, const State& right){
  left = left + right;
  return left;
}

State& operator -=(State& left, const State& right){
  left = left - right;
  return left;
}

State operator!(State t) {
  auto inverse = t;
  for(int i = 0; i < t.size(); i++){
    inverse[t[i]] = i;
  }
  return inverse;
}

bool State::isNOP(){
  for(int i = 0; i < this->size(); i++) if(this->operator[](i) != i) return false;
  return true;
}

uint64_t State::toHash() const{
	const uint64_t goldenRatioConstant = 0x9e3779b97f4a7c15;
	uint64_t result = 0;

	for ( int i = 0; i < size(); i++ ) {
		result ^= operator[]( i ) * ( ( goldenRatioConstant << i ) | ( goldenRatioConstant >> ( 64 - i ) ) );
	}

	return result;
}


