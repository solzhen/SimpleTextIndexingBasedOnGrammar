#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <sstream>

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

int notmain(int argc, char* argv[]) {
    std::string P = "bra";

    std::cout << "Program name: " << argv[0] << std::endl;

    if (argc > 1) {
        std::cout << "Pattern to find: " << argv[1] << std::endl;
        P = argv[1];
    } else {
        std::cout << "Default Pattern: bra" << std::endl;
    }

    std::string T = "abrabracadabrabra";
    std::string R = "a5471536b8cad3ra"; //"aEDGAECFbHcadCra";
    std::vector<int> L = {4, 17, 7, 9, 3, 2, 8, 2};
    int num_reglas = 8;
    int rows = num_reglas;
    int cols = num_reglas;

    std::vector<int> grid(rows * cols, 0);

    grid[3         ] = 1; // 1(A)-> a5(aE) (a        |bra     )
    grid[cols*1 + 6] = 2; // 2(B)-> 47(DG) (abrabraca|dabrabra)
    grid[cols*2 + 4] = 3; // 3(C)-> 15(AE) (abra     |bra     )
    grid[cols*3 + 5] = 4; // 4(D)-> 36(CF) (abrabra  |ca      )
    grid[cols*4 + 7] = 5; // 5(E)-> b8(bH) (b        |ra      )
    grid[cols*5    ] = 6; // 6(F)-> ca     (c        |a       )
    grid[cols*6 + 2] = 7; // 7(G)-> d3(dC) (d        |abrabra )
    grid[cols*7 + 1] = 8; // 8(H)-> ra     (r        |a       )

    // Access element at (i, j):
    // int index = i * cols + j;

    for (int t = 1; t <= P.length(); t++) {
        std::string prefix = P.substr(0, t); //P<
        std::string suffix = P.substr(t); //P>
        
        //row range
        std::string rev_pref = prefix;
        std::reverse(rev_pref.begin(), rev_pref.end());

        int s_y = findStartIndex(R, prefix);
        int e_y = findEndIndex(R, prefix);

        if (s_y <= e_y) {
            std::cout << prefix << ": " << s_y << "," << e_y << std::endl;
        } else {
            std::cout << prefix << ": not found" << std::endl;
        }

        //column range

    }

    return 0;
}