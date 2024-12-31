#include <coroutine>
#include <iostream>
#include <sdsl/bit_vectors.hpp>
#include "grid.hpp"
#include "permutations.hpp"
#include "nsequences.hpp"
#include "debug_config.hpp"
using namespace sdsl;
using namespace std;
extern "C" { // C implementation of repair and encoder
    #include "../repairs/repair110811/repair.h"
    #include "../repairs/repair110811/encoder.h"
}

extern bool SKIP;

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

/// @brief A class to search for patterns in a text
class PatternSearcher {
//private:
public:
    Grid G; // Grid
    RSequence R; // ARS sequence
    u_int S; // Initial symbol
    int_vector<> l; // Lengths of the expansion of the rules
    uint nt; // Number of terminals
    vector<unsigned char> sl; // select vector for normalized alphabet
    vector<unsigned char> rk; // rank vector for normalized alphabet
    int ruleLength(int i);    
    Generator<char> expandRuleLazy( int i, bool rev = false);
    Generator<char> expandRuleSideLazy( int i, bool left = false);
    bool compareRulesLazy(int i, int j, bool rev = false);
    string expandRule( int i, unordered_map<int, string>& memo);
    string expandRule( int i );
    string expandRightSideRule(int i, unordered_map<int, string> &memo);
    string expandLeftSideRule(int i, unordered_map<int, string>& memo);
    bool compareRulesMemoized(int i, int j, bool rev, unordered_map<int, string>& memo);
    template <typename Iterator> 
    int compareRuleWithPatternLazyImpl ( int i, Iterator pattern_begin, Iterator pattern_end, bool rev = false);
    int compareRuleWithPatternLazy(int i, string pattern, bool rev = false);
    void secondaries(vector<int> *occurences, u_int A_i, u_int offset=0, bool terminal = false);    
    int ruleAt(int r_i, int i);
    //int totalComparisons;
    //int comparisons;
    //int lastProgress;
    PatternSearcher(){};    
    /// @brief Construct a pattern searcher from a text file
    /// @param input_filename 
    PatternSearcher(string input_filename, 
        u_int*txt_len=nullptr, u_int*num_rules=nullptr, long long *bitsize=nullptr);
    /// @brief Report all occurences of a pattern in the text
    /// @param occurences Vector to store the occurences
    /// @param P Pattern to search
    void search(vector<int> *occurences, string P, vector<int> *rules_found = nullptr);
    int numRules() { return R.size() / 2; }
    int ruleAt(int i);
    long long bitsize() { 
        return G.bitsize() + R.bitsize() + size_in_bytes(l)*8 + 
            2 * sizeof(u_int)*8 + 2 * sizeof(u_char)*8 * sl.size() ; 
    }
};

