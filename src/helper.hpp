#include <string>
#include <iostream>
#include <cstdint>
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