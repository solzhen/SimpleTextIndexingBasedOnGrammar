
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

extern "C" {
    #include "../repairs/repair110811/repair.h"
    #include "../repairs/repair110811/encoder.h"
}

typedef struct _IO_FILE FILE;


std::string expandRule(const RULE* rules, int ruleIndex) {
    //std::cout << "Expanding rule: " << ruleIndex << std::endl;
    std::string expandedRule = "";
    if (ruleIndex < 256) { // we're looking at a terminal
        expandedRule += static_cast<char>(ruleIndex);
    }
    else { // we're looking at a non-terminal
        expandedRule += expandRule(rules, rules[ruleIndex].left);
        expandedRule += expandRule(rules, rules[ruleIndex].right);
    }
    return expandedRule;
}

std::string expandSequence(const CODE* sequence, const int seq_len, const RULE* rules) {
    std::string expandedSequence = "";
    for (int i = 0; i < seq_len; i++) {
        expandedSequence += expandRule(rules, sequence[i]);
    }
    return expandedSequence;
}

std::vector<uint8_t> convertStringToVector(const std::string& str) {
    std::vector<uint8_t> vec;
    for (int i = 0; i < str.size(); i++) {
        vec.push_back(static_cast<uint8_t>(str[i]));
    }
    return vec;
}

int main(int argc, char* argv[]) {


/*     const char *testing_file = "test_repair.bin";
    //std::vector<uint8_t> chars = generateRandomChars(100);
    //ontain test_str as input
    std::string test_str;
    std::cout << "Enter the input string: ";
    std::getline(std::cin, test_str);   
    //std::cin >> test_str;    
    //std:cout << "Input string: " << test_str << std::endl;
    std::vector<uint8_t> chars = convertStringToVector(test_str);
    //std::vector<uint8_t> chars = { 56, 57, 56, 57, 58, 65, 67, 68, 69, 80, 65, 67, 68, 56, 57, 58, 69, 80 };
    // print out the generated characters
    for (int i = 0; i < chars.size(); i++) {
        std::cout << static_cast<int>(chars[i]) << " ";
    }; std::cout << std::endl;
    const char *testing_file = "test_repair.bin";
    writeCharsToFile(testing_file, chars); */


    FILE *input, *output;
    DICT *dict;

    // get input filename from user
    std::string input_filename;
    std::cout << "Enter the input filename: ";
    std::cin >> input_filename;
    input  = fopen(input_filename.c_str(), "rb");

    dict = RunRepair(input);
    fclose(input);

    // print out elements of dict
    std::cout << "original text length: " << dict->txt_len << std::endl;
    std::cout << "number of rules: " << dict->num_rules - 256 << std::endl;
    std::cout << "sequence length: " << dict->seq_len << std::endl;
    //std::cout << "buff_size: " << dict->buff_size << std::endl;

    RULE *rule = dict->rule;

/*     for (int i = 0; i < dict->num_rules; i++) {
        if (rule[i].right != 256)
            std::cout << "rule[" << i << "]: " << rule[i].left << " " << rule[i].right << std::endl;
    } */

    CODE *comp_seq = dict->comp_seq;

/*     std::cout << "Sequence: ";
    for (int i = 0; i < dict->seq_len; i++) {
        std::cout << comp_seq[i] << " ";
    }
    std::cout << std::endl; */

    std::string expandedSequence = expandSequence(comp_seq, dict->seq_len, rule);
    //std::cout << "Expanded sequence: " << expandedSequence << std::endl;
    writeCharsToFile(input_filename+".bin", convertStringToVector(expandedSequence));

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

    const char *encoded_file = "test_repair.encoded.bin";
    output = fopen(encoded_file, "wb");

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

/**
 * to be done
 * Expand any rule on demand
 * Transform to grid
 * 
 * 
 */