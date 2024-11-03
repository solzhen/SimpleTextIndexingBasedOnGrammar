#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <unordered_map>

#include "helper.hpp"


using namespace std;

extern "C" { // C implementation of repair and encoder
    #include "../repairs/repair110811/repair.h"
    #include "../repairs/repair110811/encoder.h"
}


int isTerminal(char c) {
    return std::islower(c);
}

bool isPrefix(const std::string& str, const std::string& prefix) {
    return str.compare(0, prefix.length(), prefix) == 0;
}

//generate random sequence of chars
std::vector<uint8_t> generateRandomChars(int n) {
    std::vector<uint8_t> chars(n);
    for (int i = 0; i < n; i++) {
        chars[i] = rand() % 256;
    }
    return chars;
}

std::vector<uint8_t> convertStringToVector(const std::string& str) {
    std::vector<uint8_t> vec;
    for (int i = 0; i < str.size(); i++) {
        vec.push_back(static_cast<uint8_t>(str[i]));
    }
    return vec;
}




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

void rulesprinter(RULE *rules, int num_rules, bool expanded = false) {
    for (int i = 257; i < num_rules; i++) {
        std::cout << i << " -> " << rules[i].left << " " << rules[i].right << std::endl;
        if (expanded) {
            std::cout << "\t" << expand(rules, rules[i].left) << " " << expand(rules, rules[i].right) << std::endl;
        }
    }
}

