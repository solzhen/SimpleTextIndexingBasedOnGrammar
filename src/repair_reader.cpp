#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <algorithm>
#include <cstdint>

std::ifstream inputFile("your_file_name", std::ios::binary);

struct Rule
{
    uint16_t b_i;
    uint16_t c_i;
};

std::vector<Rule> rules;

#include <unordered_map>

// Define a cache to store already expanded rules
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
    } else {
        b_i += expand(rules[a_i.b_i - 256]);
    }

    if (a_i.c_i < 256) {
        c_i += (char)a_i.c_i;
    } else {
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

bool compareRules(const Rule& a_i, const Rule& a_j) {
    // Get the expanded strings
    std::string e_a_i = expand(a_i);
    std::string e_a_j = expand(a_j);
    return e_a_i < e_a_j;
}

bool compareReversedRules(const Rule& a_i, const Rule& a_j) {
    // Get the expanded strings
    std::string e_a_i = expand(a_i);
    std::string e_a_j = expand(a_j);

    // Create reversed versions of the expanded strings
    std::string reversed_e_a_i(e_a_i.rbegin(), e_a_i.rend());
    std::string reversed_e_a_j(e_a_j.rbegin(), e_a_j.rend());

    // Compare the reversed strings lexicographically
    return reversed_e_a_i < reversed_e_a_j;
}

int main()
{
    std::ifstream inputFile("rules", std::ios::binary);

    if (!inputFile.is_open())
    {
        std::cerr << "Unable to open file!" << std::endl;
        return 1;
    }
    while (!inputFile.eof())
    {
        Rule currentRule;
        inputFile.read(reinterpret_cast<char *>(&currentRule), sizeof(Rule));

        // Process the current rule
        if (inputFile.gcount() == sizeof(Rule))
        {
            rules.push_back(currentRule);
        }
    }
    inputFile.close();

        // Create a vector of indices (0 to rules.size() - 1)
    std::vector<int> indexMap(rules.size());
    std::vector<int> reverseIndexMap(rules.size());
    for (size_t i = 0; i < indexMap.size(); ++i) {
        indexMap[i] = i;
        reverseIndexMap[i] = i;
    }

    // Sort the indexMap using the custom comparator to get the mapping
    std::sort(indexMap.begin(), indexMap.end(), [&](int a, int b) {
        return compareRules(rules[a], rules[b]);
    });

    std::sort(reverseIndexMap.begin(), indexMap.end(), [&](int a, int b) {
        return compareReversedRules(rules[a], rules[b]);
    });

    // Now 'indexMap' contains the indices of rules in the sorted order
    // To get the sorted index of a rule in the original 'rules' vector, use indexMap[i]
    // For example, indexMap[3] gives the sorted index of the rule at index 3 in the original 'rules' vector

    return 0;
}