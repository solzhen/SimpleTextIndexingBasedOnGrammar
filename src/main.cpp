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


/// @brief Expands a non terminal rule
/// @param arrs ARS sequence
/// @param i index of the rule in arrs
/// @param nt number of terminals
/// @param sl de-normalized alphabet
/// @param memo memoization table
string expandRule(
    ARSSequence arrs, int i, int nt,
    vector<char> sl,
    std::unordered_map<int, string>& memo)
{
    // Check if the result for this index is already computed
    if (memo.find(i) != memo.end()) {
        return memo[i];
    }

    string left, right;
    //cout << "Expanding rule " << i << endl;
    //cout << "arrs[i]: " << arrs[i] << " arrs[i+1]: " << arrs[i+1] << endl;
    if (arrs[i] < nt) {
        left = string(1, sl[ arrs[i] + 1 ]);
    } else {        
        left = expandRule(arrs, 2*(arrs[i]-nt), nt, sl, memo);
    }
    if (arrs[i + 1] < nt) {
        right = string(1, sl[ arrs[i+1] + 1 ]);
    } else {
        right = expandRule(arrs, 2*(arrs[i+1]-nt), nt, sl, memo);
    }
    string result = left + right;
    memo[i] = result;
    return result;
}

/// @brief Expands the right side of a non terminal rule
/// @param arrs ARS sequence
/// @param i index of the rule in arrs
/// @param nt number of terminals
/// @param sl select vector
/// @param memo memoization table
string expandRightSideRule(
    ARSSequence arrs, int i, int nt,
    vector<char> sl,
    std::unordered_map<int, string>& memo) 
{
    string right;
    if (arrs[i+1] < nt) {
        right = string(1, sl[ arrs[i+1] + 1 ]);
    } else {
        right = expandRule(arrs, 2*(arrs[i+1]-nt), nt, sl, memo);
    }
    return right;
}

/// @brief Expands the left side of a non terminal rule
/// @param arrs ARS sequence
/// @param i index of the rule in arrs
/// @param nt number of terminals
/// @param sl select vector
/// @param memo memoization table
string expandLeftSideRule(
    ARSSequence arrs, int i, int nt,
    vector<char> sl,
    std::unordered_map<int, string>& memo) 
{
    string left;
    if (arrs[i] < nt) {
        left = string(1, sl[arrs[i] +1 ]);
    } else {
        left = expandRule(arrs, 2*(arrs[i]-nt), nt, sl, memo);
    }
    return left;
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
/// @param i Rule index (0-indexed)
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

/// @brief reports all occurences of pattern P in the initial symbol derived from this occurence
/// @param occurences integer vector to hold the occurences of the pattern P
/// @param R ARSSequence of normalized rules
/// @param S index of initial rule
/// @param A_i If terminal flag is false, A_i in [ 0 .. R.size()/2 ) represents
/// the index of the rule.
/// Otherwise A_i in [ 0 .. nt ) is symbol in the (normalized) alphabet
/// @param nt number of terminals/ size of the normalized alphabet
/// @param l lengths of the expanded rules
/// @param offset starting position of occurence
/// @param terminal flag to indicate if the rule is a terminal
void secondaries(vector<int> *occurences, ARSSequence R, u_int S,
    u_int A_i, u_int nt, int_vector<> l, u_int offset=0,
    bool terminal = false)
{
    if (!terminal && A_i == S) { // append the occurence to the report
        occurences->push_back(offset);
        return;
    }
    int c = terminal? A_i: A_i + nt; // symbol representing the rule
    for (int j=1; j <= R.rank(c, R.size()-1-1); j++) { // for each j-th occurence of c
        int k = R.select(c, j); // position of the j-th occurence of c
        int D_i = k / 2; // Rule index that derives the symbol at position k
        int offset_prime = offset;
        if (k % 2 == 1) { // if k is odd, then the symbol at position k is the right side of the rule D
            if ((u_int)R[k-1] < nt) 
                offset_prime += 1; // we add 1 if the left side of the rule D is a terminal
            else
                offset_prime += l[R[k-1] - nt]; // we add the length of the left side of the rule D
            //offset_prime += l[R[k-1] - nt]; // we add the length of the left side of the rule D            
        }
        secondaries(occurences, R, S, D_i, nt, l, offset_prime, false);
    }
};

/// @brief report the occurences of the pattern P in the text
/// @param occurences vector to hold the occurences of the pattern P
/// @param G Grid representation of the grammar generated by the text
/// @param R ARSSequence of normalized rules induced by re-pair
/// @param S index of the initial rule
/// @param l lengths of the expanded rules
/// @param P pattern to search
/// @param sl de-normalized alphabet select
/// @param rk de-normalized alphabet rank
/// @param nt number of terminals
/// @param mp map of the normalized alphabet
/// @param rmp reverse map of the normalized alphabet
void search(vector<int> *occurences, Grid G, ARSSequence R, u_int S, int_vector<> l, string P,
vector<char> sl, vector<char> rk, uint nt) {
    u_int m = P.size();
    if (m == 1) {
        secondaries(occurences, R, S, rk[P.at(0)], nt, l, 0, true);
    } else {
        u_int t; // length of the prefix P_< - 1
        for (t = 0; t < m-1; t++) {
            string P_left = P.substr(0, t+1); // P_<
            string P_right = P.substr(t+1, m-t-1); // P_>
            //cout << "P_left: " << P_left << " P_right: " << P_right << endl;
            uint s_x, e_x, s_y, e_y;
            int left = 0, right = G.getRows() - 1;
            int result = -1;
            while (left <= right) {
                int mid = left + (right - left) / 2;
                //cout << "r_i: " << mid << endl;
                int r_i = mid;
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

            left = 0, right = G.getRows() - 1;
            result = -1;
            while (left <= right) {
                int mid = left + (right - left) / 2;
                int r_i = mid;
                //cout << "r_i: " << mid << endl;
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

            left = 0, right = G.getColumns() - 1;
            result = -1;
            while (left <= right) {
                int mid = left + (right - left) / 2;
                int r_i = G.access(mid+1)-1; // rule index
                //cout << "mid: " << mid << endl;
                //cout << "r_i: " << r_i << endl;
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

            left = 0, right = G.getColumns() - 1;
            result = -1;
            while (left <= right) {
                int mid = left + (right - left) / 2;
                int r_i = G.access(mid+1)-1; // rule index 
                //cout << "mid: " << mid << endl;
                //cout << "r_i: " << r_i << endl;
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

            vector<Point> points = G.report(s_x, e_x, s_y, e_y); 
            for (Point p: points) {
                int r_i = p.second-1; // rule index
                if ((u_int)R[r_i*2] < nt) {
                    secondaries(occurences, R, S, r_i, nt, l, 0);
                } else {
                    secondaries(occurences, R, S, r_i, nt, l, l[R[r_i*2] - nt]-(t+1));
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

    cout << "Running Re-Pair on " << input_filename << endl;

    FILE *input;
    DICT *dict;
    input  = fopen(input_filename.c_str(), "rb");
    dict = RunRepair(input);
    fclose(input);

    std::cout << "original text length: " << dict->txt_len << std::endl;
    std::cout << "number of rules: " << dict->num_rules - 256 << std::endl;
    std::cout << "sequence length: " << dict->seq_len << std::endl;

    RULE *rules = dict->rule; // set or rules 
    CODE *comp_seq = dict->comp_seq; // sequence C
    cout << "------------------------" << endl;

    int len = dict->txt_len; // length of the text
    cout << "Text length: " << len << endl;

    //std::cout << "Sequence C: "; for (u_int i = 0; i < dict->seq_len; i++) std::cout << comp_seq[i] << " "; std::cout << std::endl;

    cout << ". . .Adding extra rules to get rid of sequence C" << endl;

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

    //std::cout << "New Sequence C: "; for (u_int i = 0; i < dict->seq_len; i++) std::cout << comp_seq[i] << " "; std::cout << std::endl;
    std::cout << "Number of rules: " << dict->num_rules - 256 << std::endl;

    cout << "------------------------" << endl;

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
        if (select_bbbb(i) > max_terminal) {
            max_terminal = select_bbbb(i);
        }
    }

    // vector to store the new normalized alphabet
    int_vector<> normalized_sequenceR(sequenceR.size(), 0, sizeof(CODE) * 8);

    int sz = sequenceR.size(); // size of the sequence R

    int r;
    int max_normalized = 0; // maximum symbol in the normalized alphabet
    for (int i = 0; i < sz; i++) {
        seqR2normalized(sequenceR[i], r);
        normalized_sequenceR[i] = r;
        if (r > max_normalized) {
            max_normalized = r;
        }
    }

    int n_terminals = rank_bbbb(257);
    int n_non_terminals = sz / 2;

    //cout << "Sequence: " << sequenceR << endl;
    //cout << "Normalized Sequence: " << normalized_sequenceR << endl;
    cout << "Max Symbol: " << max_normalized << endl;
    cout << "N. of Terminals: " << n_terminals << endl;
    cout << "N. of Non-terminals: " << n_non_terminals << endl;

    free(dict);

    ARSSequence arsSequence(normalized_sequenceR, max_normalized + 1); 
    
    cout << "------------------------" << endl;
    cout << ". . . Sorting rules" << endl;
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

    int_vector<> sortedSequenceR = int_vector(n_non_terminals * 2 + 1, 0);
    for (u_int i = 0; i < reverseIndexMap.size(); i++) {
        int a_i = reverseIndexMap[i]; // index in ARSS of the i-th smallest rule (sorted by the reversed left side)
        int b_i = arsSequence[a_i*2]; // left side of the rule
        int c_i = arsSequence[a_i*2+1]; // right side of the rule
        int n_b_i, n_c_i;
        if (b_i < n_terminals) {
            n_b_i = b_i;
        } else {
            n_b_i = distance(reverseIndexMap.begin(), find(reverseIndexMap.begin(), reverseIndexMap.end(), b_i-n_terminals))  + n_terminals;
        }
        if (c_i < n_terminals) {
            n_c_i = c_i;
        } else {
            n_c_i = distance(reverseIndexMap.begin(), find(reverseIndexMap.begin(), reverseIndexMap.end(), c_i-n_terminals))  + n_terminals;
        }
        sortedSequenceR[i*2] = n_b_i; // update the left side of the rule
        sortedSequenceR[i*2+1] = n_c_i; // update the right side of the rule
    }
    int S_i = distance(reverseIndexMap.begin(), find(reverseIndexMap.begin(), reverseIndexMap.end(), n_non_terminals-1)); // index of the initial symbol in the sorted sequence   
    cout << "Initial rule S: " << S_i << endl;
    sortedSequenceR[n_non_terminals*2] = S_i; // update the initial symbol
    ARSSequence sortedARS(sortedSequenceR, max_normalized + 1 + 1);

    cout << "------------------------" << endl;

    for (int i = 0; i < n_non_terminals; i++) {
        indexMap[i] = i;
        reverseIndexMap[i] = i;
    }
    sort(
        indexMap.begin(), 
        indexMap.end(), 
        [&](int a, int b) { 
            return compareRulesLazy(sortedARS, a, b, n_terminals, select); 
        }
    );
    sort(
        reverseIndexMap.begin(), 
        reverseIndexMap.end(), 
        [&](int a, int b) { 
            return compareRulesLazy(sortedARS, a, b, n_terminals, select, true); 
        }
    );
    cout << "Rules Sorted" << endl;
    cout << "------------------------" << endl;

    std::vector<Point> points(n_non_terminals);
    u_int j, k;
    for (u_int i = 0; i < indexMap.size(); i++) {
        k = std::distance(indexMap.begin(), std::find(indexMap.begin(), indexMap.end(), i));
        points[i] = Point(k, i);
    }

    // 0-indexed to 1-indexed
    for (u_int i = 0; i < points.size(); i++) {
        points[i].first++;
        points[i].second++;
    }
    //printPoints(points); cout << endl;

    cout << ". . . Creating Grid" << endl;
    Grid test_grid = Grid(points, n_non_terminals, n_non_terminals);

    arsSequence = sortedARS;
    cout << ". . . Precalculating lengths" << endl;

    std::unordered_map<int, string> memo;
    
    int_vector lens(n_non_terminals);
    for (int i = 0; i < n_non_terminals; i++) {
        lens[i] = expandRule(arsSequence, i*2, n_terminals, select, memo).length();
    }
    //cout << "Lengths: "; for (u_int i = 0; i < lens.size(); i++) cout << lens[i] << " "; cout << endl;

    cout << "------------------------" << endl;
/*     cout << "Rules: " << endl;
    for (int i = 0; i < n_non_terminals; i++) {
        cout << i << "-> " << arsSequence[i*2] << " " << arsSequence[i*2+1] << " -> ";
        string left = expandLeftSideRule(arsSequence, i*2, n_terminals, select, memo);
        if (left.size() < 10) cout << "<" << left << ">\t";
        else cout << "<(...)" << left.substr(left.size() - 10, 10) << ">\t";
        string right = expandRightSideRule(arsSequence, i*2, n_terminals, select, memo);
        if (right.size() < 10) cout << "<" << right << ">\t";
        else cout << "<" << right.substr(0, 10) << "(...)>";
        cout << endl;
    } */
/*     cout << "Grid:" << endl;
    for (int i = 1; i <= test_grid.getRows(); i++) {
        int j = test_grid.access(i);
        cout << i << ", " << j << " -> ";
        cout << expandLeftSideRule(arsSequence, 2*(j-1), n_terminals, select, memo) << "\t\t";
        cout << expandRightSideRule(arsSequence, 2*(j-1), n_terminals, select, memo) << endl;
    } */
    cout << "------------------------" << endl;
    string expandedSequence = expandRule(arsSequence, S_i*2, n_terminals, select, memo);
    while (true) {
        cout << "Enter the pattern to search (or exit): ";
        string pattern;
        cin >> pattern;
        if (pattern == "exit") break;
        vector<int> occurences;
        search(&occurences, test_grid, arsSequence, S_i, lens, pattern, select, rank, n_terminals);
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