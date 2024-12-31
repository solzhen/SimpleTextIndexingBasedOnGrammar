#include <string>
#include <iostream>
#include <cstdint>
#include <vector>
#include <sdsl/bit_vectors.hpp>

using namespace std;
using namespace sdsl;

extern "C" { // C implementation of repair and encoder
    #include "../repairs/repair110811/repair.h"
    #include "../repairs/repair110811/encoder.h"
}

int isTerminal(char c);
bool isPrefix(const std::string& str, const std::string& prefix);
std::vector<uint8_t> generateRandomChars(int n);
std::vector<uint8_t> convertStringToVector(const std::string& str) ;

void rulesprinter(RULE *rules, int num_rules, bool expanded);

std::string expandSequence(const CODE* sequence, const int seq_len, const RULE* rules);

#ifndef HELPER_HPP
#define HELPER_HPP

// rank_1, select_1 bit vector
typedef struct rbv {
    bit_vector b;
    rank_support_v<1, 1> rank; // rank_1
    select_support_mcl<1, 1> sel; // select_1
} rbv;
// rank_0, select_1, select_0 bit vector
typedef struct abv {
    bit_vector b;
    rank_support_v<0> rank; // rank_0
    select_support_mcl<1, 1> sel_1; // select_1
    select_support_mcl<0, 1> sel_0; // select_0
} abv;
// select_1, select_0 bit vector
typedef struct dbv {
    bit_vector b;
    select_support_mcl<1, 1> sel_1; // select_1
    select_support_mcl<0, 1> sel_0; // select_0
} dbv;

#endif // HELPER_HPP