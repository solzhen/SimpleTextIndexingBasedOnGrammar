
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <tuple>
#include <unordered_map>

#include <sdsl/bit_vectors.hpp>

#include "grid.hpp"
#include "helper.hpp"
#include "permutations.hpp"
#include "sequences.hpp"
#include <functional>
#include <boost/functional/hash.hpp>

using namespace sdsl;
using namespace std;

using CacheKey = std::pair<int, int>;
using Cache = std::unordered_map<CacheKey, std::string, boost::hash<CacheKey>>;


extern "C" { // C implementation of repair and encoder
    #include "../repairs/repair110811/repair.h"
    #include "../repairs/repair110811/encoder.h"
}

// MACROS
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

/// @brief Expands a non terminal rule
/// @param arrs ARS sequence
/// @param i index of the rule in arrs
/// @param nt number of terminals
/// @param sl de-normalized alphabet
string expandRule(
    ARSSequence arrs, int i, int nt,
    vector<char> sl) 
{
    string left, right;
    if (arrs[i] < nt) {
        left = string(1, sl[ arrs[i] + 1 ]);
    } else {        
        left = expandRule(arrs, 2*(arrs[i]-nt), nt, sl);
    }
    if (arrs[i + 1] < nt) {
        right = string(1, sl[ arrs[i+1] + 1 ]);
    } else {
        right = expandRule(arrs, 2*(arrs[i+1]-nt), nt, sl);
    }
    return left + right;
}

string expandRightSideRule(
    ARSSequence arrs, int i, int nt,
    vector<char> sl) 
{
    string right;
    if (arrs[i+1] < nt) {
        right = string(1, sl[ arrs[i+1] + 1 ]);
    } else {
        right = expandRule(arrs, 2*(arrs[i+1]-nt), nt, sl);
    }
    return right;
}

string expandLeftSideRule(
    ARSSequence arrs, int i, int nt,
    vector<char> sl) 
{
    string left;
    if (arrs[i] < nt) {
        left = string(1, sl[arrs[i] +1 ]);
    } else {
        left = expandRule(arrs, 2*(arrs[i]-nt), nt, sl);
    }
    return left;
}

/// @brief Compare two non-terminal rules
/// @param arrs 
/// @param i First rule index 
/// @param j Second rule index
/// @param nt number of non terminals
/// @param sl select vector
/// @param rev reverse flag
/// @return If reverse is set to false, comparison result between the rules' 
/// right side expansions, otherwise the comparison is 
/// done between the rules' left side reverse expansion.
bool compareRules(ARSSequence arrs, int i, int j, int nt, 
    vector<char> sl, bool rev = false) 
{
    string s_i, s_j;
    if (rev) {
        s_i = expandLeftSideRule(arrs, i*2, nt, sl);
    } else {
        s_i = expandRightSideRule(arrs, i*2, nt, sl);
    }
    if (rev) {
        s_j = expandLeftSideRule(arrs, j*2, nt, sl);
    } else {
        s_j = expandRightSideRule(arrs, j*2, nt, sl);
    }
    if (rev) {
        std::reverse(s_i.begin(), s_i.end());
        std::reverse(s_j.begin(), s_j.end());
    }
    return s_i < s_j;
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
    EncodeCFG(edict, output);
    fclose(output);
    // ------

    RULE *rules = dict->rule; // set or rules 
    CODE *comp_seq = dict->comp_seq; // sequence C

    cout << "------------------------" << endl;
    rulesprinter(rules, dict->num_rules, true);
    cout << "------------------------" << endl;

    int len = dict->txt_len - 1; // length of the text
    cout << "Text length: " << len << endl;

    dict->seq_len = dict->seq_len - 1; // remove the last element of the sequence (it's special character useless for us)
    std::cout << "Sequence C: ";
    for (u_int i = 0; i < dict->seq_len; i++) { // dict->seq_len is the length of the sequence C
        std::cout << comp_seq[i] << " ";
    } std::cout << std::endl;
    
    

    std::string expandedSequence = expandSequence(comp_seq, dict->seq_len, rules);
    std::cout << "Expanded Sequence C: " << expandedSequence << std::endl;

        /*
        First, we will get rid of sequence C, so as to have a grammar that derives T from
    a single nonterminal S. This is done by creating new nonterminals N 1 -> C[1]C[2],
    N2 -> C[3]C[4], and so on. Then we pair again N1 -> N 1 N 2 , N2 -> N 3 N4 , and so on,
recursively, until having a single nonterminal S.
        */

    while (dict->seq_len > 1) {
        for (u_int i = 0; i < dict->seq_len; i = i+2) {
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

    std::cout << "New Sequence C: ";
    for (u_int i = 0; i < dict->seq_len; i++) {
        std::cout << comp_seq[i] << " ";
    } std::cout << std::endl;
    std::cout << "Number of rules: " << dict->num_rules << std::endl;

    cout << "------------------------" << endl;
    rulesprinter(rules, dict->num_rules, true);
    cout << "------------------------" << endl;

    /*
        The set of r rules will be represented as a sequence R[1, 2r] = B1C1 B2C2 . . . BrCr .
    */

    bit_vector bbbb(257, 0); // bit vector to mark which symbols are in the alphabet used by text

    int_vector<> sequenceR((dict->num_rules - 257) * 2, 0, sizeof(CODE) * 8);
    for (u_int i = 0; i < sequenceR.size(); i = i + 2) {
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
    for (u_int i = 0; i < sequenceR.size(); i++) {
        std::cout << sequenceR[i] << " ";
    } std::cout << std::endl;

    cout << "Alphabet marker vector: " << bbbb << endl;
    rank_support_v<1> rank_bbbb(&bbbb);
    select_support_mcl<1, 1> select_bbbb(&bbbb);
    vector<char> rank(257, 0);
    vector<char> select(257, 0);
    for (int i = 0; i < 257; i++) {
        rank[i] = rank_bbbb(i);
        if (i==0) continue;
        select[i] = select_bbbb(i);
    }
    u_int max_terminal = 0;    
    for (u_int i = 1; i <= rank_bbbb(257); i++) {
        //cout << "select_bbbb(" << i << ") = " << select_bbbb(i) << endl;
        if (select_bbbb(i) > max_terminal) {
            max_terminal = select_bbbb(i);
        }
    }

    // vector to store the new normalized alphabet
    int_vector<> normalized_sequenceR(sequenceR.size(), 0, sizeof(CODE) * 8);

    cout << rank_bbbb(257) << endl;

    int sz = sequenceR.size(); // size of the sequence R

    int r;
    int max_normalized = 0;
    for (int i = 0; i < sz; i++) {
        seqR2normalized(sequenceR[i], r);
        normalized_sequenceR[i] = r;
        if (r > max_normalized) {
            max_normalized = r;
        }
    }

    int n_terminals = rank_bbbb(257);
    int n_non_terminals = sz / 2;

    cout << "Sequence: " << sequenceR << endl;
    cout << "Normalized Sequence: " << normalized_sequenceR << endl;
    cout << "Max Symbol: " << max_normalized << endl;
    cout << "N. of Terminals: " << n_terminals << endl;
    cout << "N. of Non-terminals: " << n_non_terminals << endl;

    free(dict);
    free(edict);

    ARSSequence arsSequence(normalized_sequenceR, max_normalized + 1); 

    cout << "arrs: ";
    for (u_int i = 0; i < arsSequence.size(); i++) {
        cout << arsSequence[i] << " ";
    } cout << endl;   
    
    cout << "------------------------" << endl;
    cout << "Expanding Sequence. . ." << endl;

    cout << "------------------------" << endl;
    int_vector indexMap(n_non_terminals);
    int_vector reverseIndexMap(n_non_terminals);

    for (int i = 0; i < n_non_terminals; i++) {
        indexMap[i] = i;
        reverseIndexMap[i] = i;
    }

    sort(
        indexMap.begin(), 
        indexMap.end(), 
        [&](int a, int b) { 
            return compareRules(arsSequence, a, b, n_terminals, select); 
        }
    );
    cout << "Index Map: " << endl;
    for (u_int i = 0; i < indexMap.size(); i++) cout << indexMap[i] << " "; 
    cout << endl;
    for (u_int i = 0; i < indexMap.size(); i++) {
        cout << expandRightSideRule(arsSequence, indexMap[i]*2, n_terminals, select) << ", ";
    } cout << endl;

    sort(
        reverseIndexMap.begin(), 
        reverseIndexMap.end(), 
        [&](int a, int b) { 
            return compareRules(arsSequence, a, b, n_terminals, select, true); 
        }
    );
    cout << "Reverse Index Map: " << endl;
    for (u_int i = 0; i < reverseIndexMap.size(); i++) cout << reverseIndexMap[i] << " ";
    cout << endl;
    
    for (u_int i = 0; i < reverseIndexMap.size(); i++) {
        cout << expandLeftSideRule(arsSequence, reverseIndexMap[i]*2, n_terminals, select) << ", ";
    } cout << endl;


    std::vector<Point> points(n_non_terminals);
    u_int j, k;
    for (u_int i = 0; i < indexMap.size(); i++) {
        j = indexMap[i]; // rule index
        //std::cout << "j: " << j << std::endl;
        k = std::distance(reverseIndexMap.begin(), std::find(reverseIndexMap.begin(), reverseIndexMap.end(), j));
        points[i] = Point(i, k);
    }
    cout << "Points: ";
    printPoints(points); cout << endl;

    writePointsToFile("test_grid.bin", n_non_terminals, n_non_terminals, points);

    


    return 0;
}






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
    writeCharsToFile(testing_file, chars); 


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

}

void expand(
        vector<char>* seq,
        ARSSequence arrs, 
        int i, 
        int n_terminals
    )
{
    if (arrs[i] < n_terminals) {
        seq->push_back(arrs[i]);
    } else {
        int rule_index = (arrs[i] - n_terminals) * 2;
        expand(seq, arrs, rule_index, n_terminals);
        //cout << ";;" << endl;
        expand(seq, arrs, rule_index + 1, n_terminals);
    }
}
    */