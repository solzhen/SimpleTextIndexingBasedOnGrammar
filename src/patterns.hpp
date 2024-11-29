#include <coroutine>
#include <iostream>
#include <sdsl/bit_vectors.hpp>
#include "grid.hpp"
#include "helper.hpp"
#include "permutations.hpp"
#include "sequences.hpp"
#include "debug_config.hpp"
using namespace sdsl;
using namespace std;
extern "C" { // C implementation of repair and encoder
    #include "../repairs/repair110811/repair.h"
    #include "../repairs/repair110811/encoder.h"
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

/// @brief A class to search for patterns in a text
class PatternSearcher {
private:
    Grid G; // Grid
    ARSSequence R; // ARS sequence
    u_int S; // Initial symbol
    int_vector<> l; // Lengths of the expansion of the rules
    uint nt; // Number of terminals
    vector<char> sl; // select vector for normalized alphabet
    vector<char> rk; // rank vector for normalized alphabet
    string expandRule( int i, unordered_map<int, string>& memo);
    string expandRightSideRule(int i, unordered_map<int, string> &memo);
    string expandLeftSideRule(int i, unordered_map<int, string>& memo);
    int ruleLength(int i);    
    Generator<char> expandRuleLazy( int i, bool rev = false);
    Generator<char> expandRuleSideLazy( int i, bool left = false);
    bool compareRulesLazy(int i, int j, bool rev = false);
    template <typename Iterator> 
    int compareRuleWithPatternLazyImpl ( int i, Iterator pattern_begin, Iterator pattern_end, bool rev = false);
    int compareRuleWithPatternLazy(int i, string pattern, bool rev = false);
    void secondaries(vector<int> *occurences, u_int A_i, u_int offset=0, bool terminal = false);
public:
    PatternSearcher(){};    
    /// @brief Construct a pattern searcher from a text file
    /// @param input_filename 
    PatternSearcher(string input_filename);
    /// @brief Report all occurences of a pattern in the text
    /// @param occurences Vector to store the occurences
    /// @param P Pattern to search
    void search(vector<int> *occurences, string P);
    int numRules() { return R.size() / 2; }
};

