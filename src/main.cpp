#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <sdsl/bit_vectors.hpp>

#include "grid.hpp"

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

int main(int argc, char* argv[]) {  

    string filename = "test.integers";
    if (argc < 2) {
        cout << "Generating test file test.integers" << endl;
        u32 c = 12;
        u32 r = 16;
        vector<Point> points2write = {
            {6,15},{11,1},{11,13},{4,4},{11,4},{6,3},{6,2},{4,13},
            {2,5},{2,8},{12,11},{1,6},{10,8},
            {5,10},{12,13},{7,12}
        };
        writePointsToFile("test.integers", c, r, points2write);
    } else {
        string filename = argv[1];
    }
    cout << "Reading " << filename << endl; 
    Grid grid(filename);
    cout << "count(2,11,3,9): " << grid.count(2,11,3,9) << endl;
    vector<Point> p = grid.report(2,11,3,9);
    printPoints(p);
    cout << endl;
    
    return 0;
}