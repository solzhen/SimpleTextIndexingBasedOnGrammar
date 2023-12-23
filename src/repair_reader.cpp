#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include "grid.hpp"

struct Rule {
    uint16_t b_i; // prefix of string
    uint16_t c_i; // suffix of string
};

// Define hash specialization for Rule
namespace std {
    template <>
    struct hash<Rule> {
        size_t operator()(const Rule& r) const {
            // Simple hash combining b_i and c_i
            return hash<uint16_t>()(r.b_i) ^ hash<uint16_t>()(r.c_i);
        }
    };
}
bool operator==(const Rule& lhs, const Rule& rhs) {
    return lhs.b_i == rhs.b_i && lhs.c_i == rhs.c_i;
}

std::vector<Rule> rules;

std::unordered_map<Rule, std::string> ruleCache;

std::string expand(Rule a_i) {

    // Check if the rule is already in the cache
    auto it = ruleCache.find(a_i);
    if (it != ruleCache.end()) {
        return it->second; // Return the cached result
    }

    std::string b_i;
    std::string c_i;

    if (a_i.b_i < 256) {
        b_i += (char)a_i.b_i;
    }
    else {
        b_i += expand(rules[a_i.b_i - 256]);
    }

    if (a_i.c_i < 256) {
        c_i += (char)a_i.c_i;
    }
    else {
        c_i += expand(rules[a_i.c_i - 256]);
    }

    std::ostringstream builder;
    builder << b_i;
    builder << c_i;
    std::string expandedRule = builder.str();

    // Cache the expanded rule for future use
    ruleCache[a_i] = expandedRule;

    return expandedRule;
}

std::string expand_prefix(Rule a_i) {

    std::string b_i;
    if (a_i.b_i < 256) {
        b_i += (char)a_i.b_i;
    }
    else {
        b_i += expand(rules[a_i.b_i - 256]);
    }
    return b_i;
}

std::string expand_sufix(Rule a_i) {

    std::string c_i;
    if (a_i.c_i < 256) {
        c_i += (char)a_i.c_i;
    }
    else {
        c_i += expand(rules[a_i.c_i - 256]);
    }
    return c_i;
}

bool compareRules(const Rule& a_i, const Rule& a_j, bool reverse = false) {
    std::string e_a_i;
    std::string e_a_j;
    if (reverse) {
        e_a_i = expand_prefix(a_i);
        e_a_j = expand_prefix(a_j);
        e_a_i = std::string(e_a_i.rbegin(), e_a_i.rend());
        e_a_j = std::string(e_a_j.rbegin(), e_a_j.rend());
    }
    else {
        e_a_i = expand_sufix(a_i);
        e_a_j = expand_sufix(a_j);
    }
    return e_a_i < e_a_j;
}

void createTestFile(const std::string& filename, const std::vector<Rule>& data) {
    std::ofstream outputFile(filename, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Unable to create/open file: " << filename << std::endl;
        return;
    }
    for (const auto& rule : data) {
        outputFile.write(reinterpret_cast<const char*>(&rule), sizeof(Rule));
    }
    outputFile.close();
}

void printVector(const std::vector<int>& vec, bool reverse = false) {
    if (vec.empty()) {
        std::cout << "Vector is empty." << std::endl;
        return;
    }

    std::cout << "[ ";
    for (size_t i = 0; i < vec.size() - 1; ++i) {
        if (reverse) {
            std::cout << expand_prefix(rules[vec[i]]) << ", ";
        }
        else {
            std::cout << expand_sufix(rules[vec[i]]) << ", ";
        }
    }
    if (reverse) {
        std::cout << expand_prefix(rules[vec.back()]) << " ]" << std::endl;
    }
    else {
        std::cout << expand_sufix(rules[vec.back()]) << " ]" << std::endl;
    }
}

int test() {
    // a E+256-A, D+256-A G+256-A, A+256-A E+256-A, C+256-A F+256-A, b H+256-A, c a d C+256-A r a
    std::vector<Rule> testRules = {
        {'a', 'E' + 256 - 'A'},             // A
        {'D' + 256 - 'A', 'G' + 256 - 'A'}, // B
        {'A' + 256 - 'A', 'E' + 256 - 'A'}, // C
        {'C' + 256 - 'A', 'F' + 256 - 'A'}, // D
        {'b', 'H' + 256 - 'A'},             // E
        {'c', 'a'},                         // F
        {'d', 'C' + 256 - 'A'},             // G
        {'r', 'a'}                          // H
    };

    std::string testFileName = "test_rules.bin";

    createTestFile(testFileName, testRules);

    std::ifstream inputFile("test_rules.bin", std::ios::binary);

    if (!inputFile.is_open()) {
        std::cerr << "Unable to open file!" << std::endl;
        return 1;
    }
    while (!inputFile.eof()) {
        Rule currentRule;
        inputFile.read(reinterpret_cast<char*>(&currentRule), sizeof(Rule));

        // Process the current rule
        if (inputFile.gcount() == sizeof(Rule)) {
            rules.push_back(currentRule);
        }
    }
    inputFile.close();

    std::vector<int> indexMap(rules.size());
    std::vector<int> reverseIndexMap(rules.size());
    for (size_t i = 0; i < indexMap.size(); ++i) {
        indexMap[i] = i;
        reverseIndexMap[i] = i;
    }

    // std::sort(rules.begin(), rules.end(), compareRules);
    std::sort(indexMap.begin(), indexMap.end(), [&](int a, int b) { return compareRules(rules[a], rules[b]); });

    std::sort(reverseIndexMap.begin(), reverseIndexMap.end(), [&](int a, int b) { return compareRules(rules[a], rules[b], true); });

    printVector(indexMap);
    printVector(reverseIndexMap, true);

    std::vector<Point> points(rules.size());
    uint j, k;
    for (int i = 0; i < indexMap.size(); i++) {
        j = indexMap[i]; // rule index
        k = std::distance(reverseIndexMap.begin(), std::find(reverseIndexMap.begin(), reverseIndexMap.end(), j));
        points[i] = Point(i, k);
    }

    printPoints(points);
    std::cout << std::endl;

    writePointsToFile("test_grid.bin", rules.size(), rules.size(), points);

    return 0;
}

/**
 * Now we gotta create a file
 *
 * NOW LET'S MAKE THE GRID
 * OUR GRID CLASS REQUIRES A FILE WITH THE FORMAT C (columns) R (rows) POINTS
 * so we must create the file
 *
 * each column has a point so we go column by column
 *
 * first, to know which row intersects first column, we look at our lexicographically sorted list
 *
 * if the number in first position (zeroth position) is X, then we look for X in the reverse-lexico list and return the index
 *
 * and so on:
 *  for each ith column i
 *    j <- rule index <- indexMap[i]
 *    k <- search(reverseIndexMap, j)
 *    row <- k
 *
 *
 */
