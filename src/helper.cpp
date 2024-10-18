#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <sstream>

#include "helper.hpp"

int isTerminal(char c) {
    return std::islower(c);
}

bool isPrefix(const std::string& str, const std::string& prefix) {
    return str.compare(0, prefix.length(), prefix) == 0;
}

//generate random sequence of chars
std::vector<uint8_t> generateRandomChars(int n) {
    std::vector<uint8_t> chars(n);
    for (int i = 0; i < n; i++) {
        chars[i] = rand() % 256;
    }
    return chars;
}

std::vector<uint8_t> convertStringToVector(const std::string& str) {
    std::vector<uint8_t> vec;
    for (int i = 0; i < str.size(); i++) {
        vec.push_back(static_cast<uint8_t>(str[i]));
    }
    return vec;
}










/* OUTDATED */

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
    }
    else {
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
    int right = rules.length() / 2;
    while (left < right) {

        int mid = left + (right - left) / 2;
        std::string rev_rule = expandRule(rules, mid * 2);

        if (rev_rule.compare(0, prefix.length(), prefix) < 0) {
            // The prefix is after the mid element, search in the right half
            left = mid + 1;
        }
        else {
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
        }
        else {
            // The prefix is after the mid element, search in the left half
            right = mid;
        }
    }
    return left - 1;
}


