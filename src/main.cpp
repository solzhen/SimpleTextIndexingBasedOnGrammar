
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
#include "helper.hpp"
#include "permutations.hpp"
#include "sequences.hpp"

using namespace sdsl;
using namespace std;

extern "C" { // C implementation of repair and encoder
    #include "../repairs/repair110811/repair.h"
    #include "../repairs/repair110811/encoder.h"
}

typedef struct _IO_FILE FILE;

// Hash specialization for Rule
namespace std {
    template <>
    struct hash<RULE> {
        size_t operator()(const RULE& r) const {            
            return hash<uint16_t>()(r.left) ^ hash<uint16_t>()(r.right);// Simple hash b_i and c_i
        }
    };
}
bool operator==(const RULE& lhs, const RULE& rhs) {
    return lhs.left == rhs.left && lhs.right == rhs.right;
}

std::unordered_map<RULE, std::string> ruleCache;

std::string expand(const RULE *rules, int index) {
    RULE rule = rules[index];    
    auto it = ruleCache.find(rule); // Check if the rule is already in the cache
    if (it != ruleCache.end()) { 
        return it->second; // Return the cached result
    }
    std::string leftstr;
    std::string rightstr;
    if (rule.left <= 256) {
        leftstr += (char)rule.left;
    }
    else {
        leftstr += expand(rules, rule.left);
    }
    if (rule.right <= 256) {
        rightstr = (char)rule.right;
    }
    else {
        rightstr += expand(rules, rule.right);
    }    
    ruleCache[rule] = leftstr + rightstr; // Cache the expanded rule for future use
    return ruleCache[rule];
}

std::string expand_prefix(RULE *rules, int index) {
    RULE rule = rules[index];
    return (rule.left <= 256)? std::string(1, (char)rule.left) : expand(rules, rule.left);
}

std::string expand_sufix(RULE *rules, int index) {
    RULE rule = rules[index];
    return (rule.right <= 256)? std::string(1, (char)rule.right) : expand(rules, rule.right);
}

bool compareRules(RULE *rules, int a, int b, bool reverse = false) {
    std::string expanded_rule_a;
    std::string expanded_rule_b;
    if (reverse) {
        expanded_rule_a = expand_prefix(rules, a);
        expanded_rule_b = expand_prefix(rules, b);
        expanded_rule_a = std::string(expanded_rule_a.rbegin(), expanded_rule_a.rend());
        expanded_rule_b = std::string(expanded_rule_b.rbegin(), expanded_rule_b.rend());
    }
    else {
        expanded_rule_a = expand_sufix(rules, a);
        expanded_rule_b = expand_sufix(rules, b);
    }
    return expanded_rule_a < expanded_rule_b;
}

std::string expandSequence(const CODE* sequence, const int seq_len, const RULE* rules) {
    std::string expandedSequence = "";
    for (int i = 0; i < seq_len; i++) {
        expandedSequence += expand(rules, sequence[i]);
    }
    return expandedSequence;
}

EDICT *convertDict(DICT *dict) {
  EDICT *edict = (EDICT*)malloc(sizeof(EDICT));
  uint i;
  edict->txt_len = dict->txt_len;
  edict->seq_len = dict->seq_len;
  edict->num_rules = dict->num_rules;
  edict->comp_seq = dict->comp_seq;
  edict->rule  = dict->rule;
  edict->tcode = (CODE*)malloc(sizeof(CODE)*dict->num_rules);

  for (i = 0; i <= CHAR_SIZE; i++) {
    edict->tcode[i] = i;
  }
  for (i = CHAR_SIZE+1; i < dict->num_rules; i++) {
    edict->tcode[i] = DUMMY_CODE;
  }
  return edict;
}

void rulesprinter(RULE *rules, int num_rules, bool expanded = false) {
    for (int i = 257; i < num_rules; i++) {
        std::cout << i << " -> " << rules[i].left << " " << rules[i].right << std::endl;
        if (expanded) {
            std::cout << "\t" << expand(rules, rules[i].left) << " " << expand(rules, rules[i].right) << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {

    std::string input_filename;
    if (argc < 2) {
        std::cout << "Enter the input filename: ";
        std::cin >> input_filename;
    }
    else {
        input_filename = argv[1];
    }

    FILE *input, *output;
    DICT *dict;
    input  = fopen(input_filename.c_str(), "rb");
    dict = RunRepair(input);
    fclose(input);

    std::cout << "original text length: " << dict->txt_len << std::endl;
    std::cout << "number of rules: " << dict->num_rules - 256 << std::endl;
    std::cout << "sequence length: " << dict->seq_len << std::endl;

    // ------ this next part is just to test file size after encoding
    EDICT *edict = convertDict(dict);
    std::string encoded_file = input_filename + ".encoded.bin";
    output = fopen(encoded_file.c_str(), "wb");
    //std::cout << "Encoding..." << std::endl;
    EncodeCFG(edict, output);
    fclose(output);
    // ------

    RULE *rules = dict->rule; // set or rules 
    CODE *comp_seq = dict->comp_seq; // sequence C

    rulesprinter(rules, dict->num_rules, true);

    dict->seq_len = dict->seq_len - 1; // remove the last element of the sequence (it's special character useless for us)
    //print sequence C
    std::cout << "Sequence C: ";
    for (int i = 0; i < dict->seq_len; i++) { // dict->seq_len is the length of the sequence C
        std::cout << comp_seq[i] << " ";
    } std::cout << std::endl;

    //print expanded sequence C
    std::string expandedSequence = expandSequence(comp_seq, dict->seq_len, rules);
    std::cout << "Expanded Sequence C: " << expandedSequence << std::endl;

        /*
        First, we will get rid of sequence C, so as to have a grammar that derives T from
    a single nonterminal S. This is done by creating new nonterminals N 1 -> C[1]C[2],
    N2 -> C[3]C[4], and so on. Then we pair again N1 -> N 1 N 2 , N2 -> N 3 N4 , and so on,
recursively, until having a single nonterminal S.
        */

    while (dict->seq_len > 1) {
        for (int i = 0; i < dict->seq_len; i = i+2) {
            if (i == dict->seq_len - 1) { // if we're at the last element of the sequence, it means the sequence length was odd
                comp_seq[i/2] = comp_seq[i];
            }
            else { // This works, but in the future, I may need to expose the AddNewPair function from repair.c.
                RULE new_rule;
                new_rule.left = comp_seq[i];
                new_rule.right = comp_seq[i+1];
                rules[dict->num_rules] = new_rule; // append the new rule to the rules
                comp_seq[i/2] = dict->num_rules; // update the sequence C
                dict->num_rules++; // increment the number of rules
            }
        }
        dict->seq_len = dict->seq_len % 2 == 0 ? dict->seq_len / 2 : dict->seq_len / 2 + 1; // update the sequence length
    }

    //print new sequence C
    std::cout << "New Sequence C: ";
    for (int i = 0; i < dict->seq_len; i++) {
        std::cout << comp_seq[i] << " ";
    } std::cout << std::endl;
    std::cout << "Number of rules: " << dict->num_rules << std::endl;

    //print new rules
    rulesprinter(rules, dict->num_rules, true);

    /*
        The set of r rules will be represented as a sequence R[1, 2r] = B1C1 B2C2 . . . BrCr .
    */

    bit_vector bbbb(257, 0);

    int_vector<> sequenceR((dict->num_rules - 257) * 2, 0, sizeof(CODE) * 8);
    for (int i = 0; i < sequenceR.size(); i = i + 2) {
        sequenceR[i] = rules[i/2 + 257].left;
        sequenceR[i + 1] = rules[i/2 + 257].right;
        if (sequenceR[i] <= 256) {
            bbbb[sequenceR[i]] = 1;
        }
        if (sequenceR[i + 1] <= 256) {
            bbbb[sequenceR[i + 1]] = 1;
        }
    }

    //print sequence R
    std::cout << "Sequence R: ";
    for (int i = 0; i < sequenceR.size(); i++) {
        std::cout << sequenceR[i] << " ";
    } std::cout << std::endl;

    cout << bbbb << endl;
    rank_support_v<1> rank_bbbb(&bbbb);
    select_support_mcl<1, 1> select_bbbb(&bbbb);
    int max_terminal = 0;
    for (int i = 1; i <= rank_bbbb(257); i++) {
        cout << "select_bbbb(" << i << ") = " << select_bbbb(i) << endl;
        if (select_bbbb(i) > max_terminal) {
            max_terminal = select_bbbb(i);
        }
    }

    int_vector<> normalized_sequenceR(sequenceR.size(), 0, sizeof(CODE) * 8);
    cout << normalized_sequenceR << endl;

    cout << rank_bbbb(257) << endl;

    int sz = sequenceR.size();
    
    #define seqR2normalized(c, result) \
        do { \
            if (c < 256)  \
                (result) = rank_bbbb(c + 1) - 1; \
            else\      
                (result) = c - 257 + rank_bbbb(257); \
        } while (0)
    #define normalized2seqR(c, result) \
        do { \
            if (c < rank_bbbb(257)) \
                (result) = select_bbbb(c + 1); \
            else \
                (result) = c - rank_bbbb(257) + 257; \
        } while (0)

    int r;
    int max_normalized = 0;
    for (int i = 0; i < sz; i++) {
        seqR2normalized(sequenceR[i], r);
        normalized_sequenceR[i] = r;
        if (r > max_normalized) {
            max_normalized = r;
        }
    }

    cout << sequenceR << endl;
    cout << normalized_sequenceR << endl;
    //cout << max_normalized << endl;


    free(dict);
    free(edict);

    ARSSequence arsSequence(normalized_sequenceR, max_normalized + 1);
    
    for (int i = 0; i < normalized_sequenceR.size(); i++) {
        cout << "access(" << i << ") = " << arsSequence.access(i) << endl;
    }


    

    /* ------------------- check repair_reader.test() for ideas on converting to grid ------------*/



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



    /*
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

    Grid test_grid("test_grid.bin");
    */