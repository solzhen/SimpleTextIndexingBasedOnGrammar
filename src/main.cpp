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
#include <coroutine>

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
#include <optional>
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

void saveIntVector(const sdsl::int_vector<>& iv, const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    if (out.is_open()) {
        iv.serialize(out);
        out.close();
    } else {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
    }
}
void loadIntVector(sdsl::int_vector<>& iv, const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (in.is_open()) {
        iv.load(in);
        in.close();
    } else {
        std::cerr << "Failed to open file for reading: " << filename << std::endl;
    }
}

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

/// @brief Expands the right side of a non terminal rule
/// @param arrs ARS sequence
/// @param i index of the rule in arrs
/// @param nt number of terminals
/// @param sl select vector
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

/// @brief Expands the left side of a non terminal rule
/// @param arrs ARS sequence
/// @param i index of the rule in arrs
/// @param nt number of terminals
/// @param sl select vector
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
    cout << "Comparing rules " << i << " and " << j << " rev: " << rev << endl;
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

template <typename T>
struct Generator {
    struct promise_type {
        T current_value;
        auto get_return_object() { return Generator{this}; }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T value) {
            current_value = value;
            return {};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    struct iterator {
        std::coroutine_handle<promise_type> coro;

        iterator& operator++() {
            coro.resume();
            if (coro.done()) coro = nullptr;
            return *this;
        }
        T operator*() const { return coro.promise().current_value; }
        bool operator!=(const iterator& other) const { return coro != other.coro; }
        bool operator==(const iterator& other) const { return coro == other.coro; }
    };

    iterator begin() {
        if (coro) coro.resume();
        return iterator{coro};
    }
    iterator end() { return iterator{nullptr}; }

    explicit Generator(promise_type* p) : coro(std::coroutine_handle<promise_type>::from_promise(*p)) {}
    ~Generator() {
        if (coro) coro.destroy();
    }

    std::coroutine_handle<promise_type> coro;
};

/// @brief Lazily expands a non-terminal rule and yields one character at a time. If
/// the reverse flag is set, the expansion is done in reverse order.
/// @param arrs ARS sequence
/// @param i index of the rule in arrs
/// @param nt number of terminals
/// @param sl de-normalized alphabet
/// @param rev reverse flag
Generator<char> expandRuleLazy(
    ARSSequence& arrs, int i, int nt,
    std::vector<char>& sl, bool rev = false)
{
    int f,s;
    f = rev? i+1: i;
    s = rev? i: i+1;    
    // expand left side if not reverse, otherwise expand right side
    if (arrs[f] < nt) { 
        co_yield sl[arrs[f] + 1];
    } else {
        auto first_gen = expandRuleLazy(arrs, 2*(arrs[f]-nt), nt, sl, rev);
        for (char c : first_gen) {
            co_yield c;
        }
    }
    // expand right side if not reverse, otherwise expand left side
    if (arrs[s] < nt) {
        co_yield sl[arrs[s] + 1];
    } else {
        auto second_gen = expandRuleLazy(arrs, 2*(arrs[s]-nt), nt, sl, rev);
        for (char c : second_gen) {
            co_yield c;
        }
    }
}
/// @brief Lazily expands one of the sides of a non-terminal rule and yields one character at a time.
/// If the left flag is set, the left side is expanded, otherwise the right side is expanded.
/// The left side expansion is done in reverse order.
/// @param arrs 
/// @param i 
/// @param nt 
/// @param sl 
/// @param left 
/// @return 
Generator<char> expandRuleSideLazy(
    ARSSequence& arrs, int i, int nt,
    std::vector<char>& sl, bool left = false)
{    
    int lr_i = left? i: i+1;
    if (arrs[lr_i] < nt) {
        co_yield sl[arrs[lr_i] + 1];
    } else {
        auto gen = expandRuleLazy(arrs, 2*(arrs[lr_i]-nt), nt, sl, left);
        for (char c : gen) {
            co_yield c;
        }
    }
}

bool compareRulesLazy(ARSSequence& arrs, int i, int j, int nt, std::vector<char>& sl, bool rev = false) 
{
    // Set up generators for each rule's expansion
    auto gen_i = expandRuleSideLazy(arrs, 2 * i, nt, sl, rev);
    auto gen_j = expandRuleSideLazy(arrs, 2 * j, nt, sl, rev);
    // Create iterators to lazily consume characters from both expansions
    auto it_i = gen_i.begin();
    auto it_j = gen_j.begin();
    // Compare characters from both sequences one by one
    while (it_i != gen_i.end() && it_j != gen_j.end()) {
        char char_i = *it_i;
        char char_j = *it_j;
        if (char_i != char_j) {
            return char_i < char_j;
        }
        ++it_i;
        ++it_j;
    }
    // If one sequence is shorter, the shorter one is considered "less"
    return (it_i == gen_i.end()) && (it_j != gen_j.end());
}

template <typename Iterator>
int compareRuleWithPatternLazyImpl(
    ARSSequence& arrs, int i, int nt, std::vector<char>& sl, Iterator pattern_begin, Iterator pattern_end,
    bool rev = false) 
{      
    auto gen = expandRuleSideLazy(arrs, 2*i, nt, sl, rev);
    auto it = gen.begin();
    while (it != gen.end() && pattern_begin != pattern_end) {
        char c = *it;
        char p = *pattern_begin;
        //cout << "c: " << c << " p: " << p << endl;
        if (c < p) return -1; // if the character in the expansion is less than the pattern character
        if (c > p) return 1;  // if the character in the expansion is greater than the pattern character        
        ++it;
        ++pattern_begin;
    }
    if (it == gen.end() && pattern_begin != pattern_end) return -1;  // Expansion ended first
    return 0;  // if the expansion is equal to the pattern or the pattern is a prefix of the expansion
}

/// @brief Compare the expansion of a rule with a pattern lazily
/// @param arrs Access, Rank, Select supporting sequence
/// @param i Rule index
/// @param nt Number of terminals
/// @param sl Select vector
/// @param pattern 
/// @param rev 
/// @return 0 if the pattern is a prefix of the expanded rule, 1 if the rule expansion is bigger than the pattern, -1 if less, 
int compareRuleWithPatternLazy(ARSSequence& arrs, int i, int nt, std::vector<char>& sl, std::string pattern, bool rev = false) 
{  
    //cout << "Comparing rule " << i << " with pattern " << pattern << " rev: " << rev << endl; 
    if (rev) {
        return compareRuleWithPatternLazyImpl(arrs, i, nt, sl, pattern.rbegin(), pattern.rend(), rev);
    } else {
        return compareRuleWithPatternLazyImpl(arrs, i, nt, sl, pattern.begin(), pattern.end(), rev);
    }
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

/// @brief reports all occurences of pattern P in the initial symbol derived from this occurence
/// @param occurences integer vector to hold the occurences of the pattern P
/// @param R ARSSequence of normalized rules
/// @param A_i If terminal flag is false, A_i in [ 0 .. R.size()/2 ) represents
/// the index of the rule.
/// Otherwise A_i in [ 0 .. nt ) is symbol in the (normalized) alphabet
/// @param nt number of terminals/ size of the normalized alphabet
/// @param l lengths of the expanded rules
/// @param offset starting position of occurence
/// @param terminal flag to indicate if the rule is a terminal
void secondaries(vector<int> *occurences, ARSSequence R, 
    u_int A_i, u_int nt, int_vector<> l, u_int offset=0,
    bool terminal = false)
{
    //cout << "A_i: " << A_i << " offset: " << offset << endl;
    if (!terminal && A_i == R.size()/2-1) { // append the occurence to the report
        occurences->push_back(offset);
        return;
    }
    int c = terminal? A_i: A_i + nt; // symbol representing the rule
    //cout << "c: " << c << endl;
    //cout << "number of occurences of c: " << R.rank(c, R.size()) << endl;
    for (int j=1; j <= R.rank(c, R.size()); j++) { // for each j-th occurence of c
        int k = R.select(c, j); // position of the j-th occurence of c
        //cout << "j: " << j << ", k: " << k << endl;
        int D_i = k / 2; // Rule index that derives the symbol at position k
        int offset_prime = offset;
        if (k % 2 == 1) { // if k is odd, then the symbol at position k is the right side of the rule D
            if ((u_int)R[k-1] < nt) 
                offset_prime += 1; // we add 1 if the left side of the rule D is a terminal
            else
                offset_prime += l[R[k-1] - nt]; // we add the length of the left side of the rule D
            //offset_prime += l[R[k-1] - nt]; // we add the length of the left side of the rule D            
        }
        //cout << "D_i: " << D_i << " offset_prime: " << offset_prime << endl;
        secondaries(occurences, R, D_i, nt, l, offset_prime, false);
    }
};

/// @brief report the occurences of the pattern P in the text
/// @param occurences vector to hold the occurences of the pattern P
/// @param G Grid representation of the grammar generated by the text
/// @param R ARSSequence of normalized rules induced by re-pair
/// @param l lengths of the expanded rules
/// @param P pattern to search
/// @param sl de-normalized alphabet select
/// @param rk de-normalized alphabet rank
/// @param nt number of terminals
/// @param mp map of the normalized alphabet
/// @param rmp reverse map of the normalized alphabet
void search(vector<int> *occurences, Grid G, ARSSequence R, int_vector<> l, string P,
vector<char> sl, vector<char> rk, uint nt, int_vector<> mp, int_vector<> rmp) {
    u_int m = P.size();
    if (m == 1) {
        secondaries(occurences, R, rk[P.at(0)], nt, l, 0, true);
    } else {
        for (u_int t = 0; t < m-1; t++) {
            // First we cut P into two parts P_< and P_>
            string P_left = P.substr(0, t+1); // P_<
            string P_right = P.substr(t+1, m-t-1); // P_>
            //cout << "P_left: " << P_left << " P_right: " << P_right << endl;
            uint s_x, e_x, s_y, e_y;
            // find the range of rows [s_y, e_y] that finish with P_<,
            // these form a range because they are sorted in lexicographic order of
            // the reverse string, and the range can be found using binary search of 
            // the reversed prefix of P (P_<_reversed)
            
            // Binary search to find the first index of a string that starts with P_<_reversed
            int left = 0, right = G.getRows() - 1;
            int result = -1;
            while (left <= right) {
                int mid = left + (right - left) / 2;
                //cout << "mid: " << mid << endl;
                int r_i = rmp[mid]; // rule index
                // Compare the expansion with the pattern lazily
                int compare = compareRuleWithPatternLazy(R, r_i, nt, sl, P_left, true);
                if (compare >= 0) { // if the expansion is greater or equal to the pattern
                    if (compare == 0) { // if the expansion is equal to the pattern or the pattern is a prefix of the expansion
                        result = mid;  // potential match, move left to find the first occurrence
                    }
                    right = mid - 1;
                } else {
                    left = mid + 1;
                }
            }
            if (result == -1) continue;
            s_y = result + 1; //we add 1 since the grid is 1-indexed
            //cout << "s_y: " << s_y << endl;

            // Binary search to find the last index of a string that starts with P_<_reversed
            left = 0, right = G.getRows() - 1;
            result = -1;
            while (left <= right) {
                int mid = left + (right - left) / 2;
                //cout << "mid: " << mid << endl;
                int r_i = rmp[mid]; // rule index
                int compare = compareRuleWithPatternLazy(R, r_i, nt, sl, P_left, true);
                if (compare <= 0) { // if the expansion is less or equal to the pattern
                    if (compare == 0) { // if the expansion is equal to the pattern or the pattern is a prefix of the expansion
                        result = mid;  // potential match, move right to find the last occurrence
                    }
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }
            if (result == -1) continue;
            e_y = result + 1;
            //cout << "e_y: " << e_y << endl;
            // find the range of columns [s_x, e_x] that start with P_>,
            // these form a range because they are sorted in lexicographic order
            // and the range can be found using binary search of the prefix of P (P_>)
            // Binary search to find the first index of a string that starts with P_>
            left = 0, right = G.getColumns() - 1;
            result = -1;
            while (left <= right) {
                int mid = left + (right - left) / 2;
                //cout << "mid: " << mid << endl;
                int r_i = mp[mid]; // rule index
                int compare = compareRuleWithPatternLazy(R, r_i, nt, sl, P_right);
                if (compare >= 0) { // if the expansion is greater or equal to the pattern
                    if (compare == 0) { // if the expansion is equal to the pattern or the pattern is a prefix of the expansion
                        result = mid;  // potential match, move left to find the first occurrence
                    }
                    right = mid - 1;
                } else {
                    left = mid + 1;
                }
            }
            if (result == -1) continue;
            s_x = result + 1; 
            //cout << "s_x: " << s_x << endl;

            // Binary search to find the last index of a string that starts with P_>
            left = 0, right = G.getColumns() - 1;
            result = -1;
            while (left <= right) {
                int mid = left + (right - left) / 2;
                //cout << "mid: " << mid << endl;
                int r_i = mp[mid]; // rule index
                int compare = compareRuleWithPatternLazy(R, r_i, nt, sl, P_right);
                if (compare <= 0) { // if the expansion is less or equal to the pattern
                    if (compare == 0) { // if the expansion is equal to the pattern or the pattern is a prefix of the expansion
                        result = mid;  // potential match, move right to find the last occurrence
                    }
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }
            if (result == -1) continue;
            e_x = result + 1;
            //cout << "e_x: " << e_x << endl;

            //cout << "s_x: " << s_x << " e_x: " << e_x << " s_y: " << s_y << " e_y: " << e_y << endl;
            vector<Point> points = G.report(s_x, e_x, s_y, e_y);
            
            //printPoints(points);
            //cout << endl;
            for (Point p: points) {
                int r_i = mp[p.first-1]; // rule index
                //cout << "r_i: " << r_i << endl;
                //cout << "R: " << R[r_i*2] << " " << R[r_i*2+1] << endl;
                if ((u_int)R[r_i*2] < nt) {
                    secondaries(occurences, R, r_i, nt, l, 0);
                } else {
                    //cout << l << endl;
                    secondaries(occurences, R, r_i, nt, l, l[R[r_i*2] - nt] -t-1);
                }
            }
        }
    }
};



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
    //rulesprinter(rules, dict->num_rules, true);
    cout << "------------------------" << endl;

    int len = dict->txt_len; // length of the text
    cout << "Text length: " << len << endl;

    dict->seq_len = dict->seq_len;
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
    //rulesprinter(rules, dict->num_rules, true);
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

    //cout << "Alphabet marker vector: " << bbbb << endl;
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


    
    cout << "------------------------" << endl;

    //for (int i = 0; i < n_non_terminals; i++) {
    //    cout << i << ":" << i+n_terminals << "\t->\t" << arsSequence[i*2] << '\t' << arsSequence[i*2+1] << endl;
    //} 
    cout << ". . . sorting rules" << endl;
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
            return compareRulesLazy(arsSequence, a, b, n_terminals, select); 
        }
    );
    sort(
        reverseIndexMap.begin(), 
        reverseIndexMap.end(), 
        [&](int a, int b) { 
            return compareRulesLazy(arsSequence, a, b, n_terminals, select, true); 
        }
    );
    std::vector<Point> points(n_non_terminals);
    u_int j, k;
    for (u_int i = 0; i < indexMap.size(); i++) {
        j = indexMap[i]; // rule index
        k = std::distance(reverseIndexMap.begin(), std::find(reverseIndexMap.begin(), reverseIndexMap.end(), j));
        points[i] = Point(i, k);
    }

    saveIntVector(indexMap, input_filename+".im.bin");
    saveIntVector(reverseIndexMap, input_filename+"rim.bin");

    // 0-indexed to 1-indexed
    for (u_int i = 0; i < points.size(); i++) {
        points[i].first++;
        points[i].second++;
    }

    cout << ". . . Writing grid to file" << endl;

    string grid_filename = input_filename + ".grid.bin";

    writePointsToFile(grid_filename, n_non_terminals, n_non_terminals, points);
    Grid test_grid = Grid(grid_filename);

    //test_grid.printself(); 

    //int c = test_grid.count(1, 8, 1, 8);
    //cout << "count(1, 8, 1, 8): " << c << endl;

    //cout << "report(1, 8, 1, 4): ";
    //vector<Point> p = test_grid.report(1, 8, 1, 4);
    //printPoints(p); cout << endl;

    //cout << "report(1, 4, 1, 4): ";
    //p = test_grid.report(1, 4, 1, 4);
    //printPoints(p); cout << endl;

    int_vector lens(n_non_terminals);
    for (int i = 0; i < n_non_terminals; i++) {
        lens[i] = expandRule(arsSequence, i*2, n_terminals, select).length();
    }
    cout << "Lengths: ";
    for (u_int i = 0; i < lens.size(); i++) {
        cout << lens[i] << " ";
    } cout << endl;   

    cout << "------------------------" << endl;    
    
    expandedSequence = expandRule(arsSequence, (n_non_terminals-1)*2, n_terminals, select);

    while (true) {
        cout << "Enter the pattern to search (or exit): ";
        string pattern;
        cin >> pattern;
        if (pattern == "exit") break;
        vector<int> occurences;
        search(&occurences, test_grid, arsSequence, lens, pattern, select, rank, n_terminals, indexMap, reverseIndexMap);
        cout << "Occurences Found: \t";
        sort(occurences.begin(), occurences.end());
        for (u_int i = 0; i < occurences.size(); i++) {
            cout << occurences[i] << " ";
        } cout << endl;
        cout << "Expected Occurences: \t";
        for (u_int i = 0; i < expandedSequence.size(); i++) {
            if (expandedSequence.substr(i, pattern.length()) == pattern) {
                cout << i << " ";
            }
        } cout << endl;
    }
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