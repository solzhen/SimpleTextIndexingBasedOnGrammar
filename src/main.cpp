#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <sdsl/bit_vectors.hpp>

#include "wavelet_matrix.hpp"

using namespace sdsl;
using namespace std;

int isTerminal(char c) {
    return std::islower(c);
}

bool isPrefix(const std::string& str, const std::string& prefix) {
    return str.compare(0, prefix.length(), prefix) == 0;
}


/// @brief given a set of rules A -> BC represented by the
/// string of concatenated BC pairs, expands component pointed at 
/// by the index given         
/// @param rules string of pairs of symbols
/// @param index 0-indexed index pointing at the rule
/// @return 
std::string expandRule(const std::string& rules, int index) {

    std::stringstream expandedString;

    char currentSymbol = rules[index];
    if (isTerminal(currentSymbol)) {
        expandedString << currentSymbol;
    } else {
        int nextRuleIndexB = (currentSymbol - '0') * 2 - 2;
        int nextRuleIndexC = (currentSymbol - '0') * 2 - 1;
        expandedString << expandRule(rules, nextRuleIndexB);
        expandedString << expandRule(rules, nextRuleIndexC);
    }
    return expandedString.str();
}

// Find the starting index of the range of rows with prefix given
int findStartIndex(const std::string& rules, const std::string& prefix) {
    int left = 0;
    int right = rules.length() / 2 ;
    while (left < right) {

        int mid = left + (right - left) / 2;
        std::string rev_rule = expandRule(rules, mid * 2);

        if (rev_rule.compare(0, prefix.length(), prefix) < 0) {
            // The prefix is after the mid element, search in the right half
            left = mid + 1;
        } else {
            // The prefix is at or before the mid element, search in the left half
            right = mid;
        }
    }
    return left;
}

// Find the ending index of the range of rows with given prefix
int findEndIndex(const std::string& rules, const std::string& prefix) {
    int left = 0;
    int right = rules.length() / 2;

    while (left < right) {
        int mid = left + (right - left) / 2;
        std::string rev_rule = expandRule(rules, mid * 2);

        if (rev_rule.compare(0, prefix.length(), prefix) <= 0) {
            // The prefix is before or at the mid element, search in the right half
            left = mid + 1;
        } else {
            // The prefix is after the mid element, search in the left half
            right = mid;
        }
    }

    return left - 1;
}

int main() {
    vector<u32> seq = {2,3,9,7,5, 9,4,2,8,6, 3,7,5,7,7, 1,3,10,6,1, 4,1,3,1,1};
    vector<u32> seq_c(seq);
    int s = seq.size();
    WaveletMatrix wm(seq, 10);
    wm.printself();
}