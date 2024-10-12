
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
#include "repair_reader.hpp"
//#include "re-pair.hpp"

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

extern "C" {
    #include "../repairs/repair110811/repair.h"
    #include "../repairs/repair110811/encoder.h"
}


typedef struct _IO_FILE FILE;

// write a vector of characters to a file
void writeCharsToFile(const std::string& filename, const std::vector<uint8_t>& chars) {
    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }
    outfile.write(reinterpret_cast<const char*>(chars.data()), chars.size());
    outfile.close();
    if (!outfile) {
        std::cerr << "Error writing to file: " << filename << std::endl;
    }
}


//generate random sequence of chars
std::vector<uint8_t> generateRandomChars(int n) {
    std::vector<uint8_t> chars(n);
    for (int i = 0; i < n; i++) {
        chars[i] = rand() % 256;
    }
    return chars;
}




int main(int argc, char* argv[]) {

    //std::vector<uint8_t> chars = generateRandomChars(100);

    std::vector<uint8_t> chars = { 56, 57, 56, 57, 58, 65, 67, 68, 69, 80, 65, 67, 68, 56, 57, 58, 69, 80 };
    // print out the generated characters
    for (int i = 0; i < chars.size(); i++) {
        std::cout << static_cast<int>(chars[i]) << " ";
    }; std::cout << std::endl;
    writeCharsToFile("test_repair.bin", chars);

    FILE *input, *output;
    DICT *dict;

    input  = fopen("test_repair.bin", "r");

    dict = RunRepair(input);
    fclose(input);

    // print out elements of dict
    std::cout << "original text length: " << dict->txt_len << std::endl;
    std::cout << "number of rules: " << dict->num_rules - 256 << std::endl;
    std::cout << "sequence length: " << dict->seq_len << std::endl;
    //std::cout << "buff_size: " << dict->buff_size << std::endl;

    RULE *rule = dict->rule;
    for (int i = 0; i < dict->num_rules; i++) {
        if (rule[i].right != 256)
            std::cout << "rule[" << i << "]: " << rule[i].left << " " << rule[i].right << std::endl;
    }
    CODE *comp_seq = dict->comp_seq;
    std::cout << "Sequence: ";
    for (int i = 0; i < dict->seq_len; i++) {
        std::cout << comp_seq[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Converting dictionary..." << std::endl;

    EDICT *edict = (EDICT*)malloc(sizeof(EDICT));
    edict->txt_len = dict->txt_len;
    edict->seq_len = dict->seq_len;
    edict->num_rules = dict->num_rules;
    edict->comp_seq = dict->comp_seq;
    edict->rule  = dict->rule;
    edict->tcode = (CODE*)malloc(sizeof(CODE)*dict->num_rules);
    for (int i = 0; i <= CHAR_SIZE; i++) {
        edict->tcode[i] = i;
    }    
    for (int i = CHAR_SIZE+1; i < dict->num_rules; i++) {
        edict->tcode[i] = DUMMY_CODE;
    }

    output = fopen("test_repair.bout", "wb");

    std::cout << "Encoding..." << std::endl;
    EncodeCFG(edict, output);
    free(dict);
    free(edict);
    fclose(output);

    string filename = "test.integers";
    if (argc < 2) {
        cout << "Generating test file test.integers" << endl;
        u32 c = 12;
        u32 r = 16;
        vector<Point> points2write = {
            {6, 15}, {11, 1}, {11, 13}, {4, 4}, {11, 4}, {6, 3}, {6, 2}, {4, 13}, {2, 5}, {2, 8}, {12, 11}, {1, 6}, {10, 8}, {5, 10}, {12, 13}, {7, 12} };
        writePointsToFile("test.integers", c, r, points2write);
    }
    else {
        string filename = argv[1];
    }
    cout << "Reading " << filename << endl;
    Grid grid(filename);
    cout << "count(2,11,3,9): " << grid.count(2, 11, 3, 9) << endl;
    vector<Point> p = grid.report(2, 11, 3, 9);
    printPoints(p);
    cout << endl;

    test();

    Grid test_grid("test_grid.bin");

    /*     // Re Pair quick test
        std::string input;
        std::cout << "Enter the input string: ";
        std::cin >> input;
        auto compressed = rePairCompression(input);
        std::cout << "Compressed: " << compressed.first << std::endl;
        std::cout << "Dictionary of Symbols:" << std::endl;
        for (const auto& entry : compressed.second) {
            std::cout << entry.first << " -> " << entry.second << std::endl;
        }
        std::string decompressed = decompress(compressed.first, compressed.second);
        std::cout << "Decompressed: " << decompressed << std::endl; */


    return 0;
}