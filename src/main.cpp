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
#include <chrono>
#include <cxxopts.hpp>
#include <sys/stat.h>
#include "npatterns.hpp"

using namespace sdsl;
using namespace std;

typedef struct { 
    int left,right;
} Tpair;

string expandRule(Tpair *R, int i, unordered_map<int, string> &memo, int alph, char* map) {
    if (memo.find(i) != memo.end()) {
        return memo[i];
    }
    string left, right;
    if (R[i].left < alph) {
        left = map[ R[i].left ];
    } else {        
        left = expandRule(R, R[i].left-alph, memo, alph, map);
    }
    if (R[i].right < alph) {
        right = map[ R[i].right ];
    } else {
        right = expandRule(R, R[i].right-alph, memo, alph, map);
    }    
    string result = left + right;
    memo[i] = result;
    return result;
}

typedef struct {
    string pattern;
    int occurrences;
    vector<int> positions;
    double time;
} PatternResult;

int main(int argc, char* argv[]) {
    std::string input_filename;
    cxxopts::Options options("Pattern Searcher", "Search for text patterns in a text file");
    options.add_options()
        ("f,file", "Input file name", cxxopts::value<std::string>())
        ("d,debug", "Enable debugging mode")
        ("t,time", "Print execution time")
        ("h,help", "Print usage")
        ("s,skip", "Skip the sorting of the rules")
        ("p,patterns", "File with patterns to search", cxxopts::value<std::string>())
        ("o,output", "Output file name", cxxopts::value<std::string>());
        
    auto result = options.parse(argc, argv);
    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }
    if (result.count("file")) {
        input_filename = result["file"].as<std::string>();
        std::cout << "Input file: " << input_filename << std::endl;
    } else {
        std::cout << "Enter the input filename: ";
        std::cin >> input_filename;
    }
    if (result["debug"].as<bool>()) {
        std::cout << "Debug mode enabled." << std::endl;
        DEBUG = true;
    }
    if (result["time"].as<bool>()) {
        std::cout << "Execution time will be printed." << std::endl;
        TIME = true;
    }
    if (result["skip"].as<bool>()) {
        std::cout << "Skipping the sorting of the rules." << std::endl;
        SKIP = true;
    }

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    long long bit_s; 
    u_int txt_len, num_rules;

    auto t1 = high_resolution_clock::now();    
    nPatternSearcher PS(input_filename, &txt_len, &num_rules, &bit_s);
    auto t2 = high_resolution_clock::now();
    if (TIME) {
        duration<double, std::milli> ms_double = t2 - t1;
        std::cout << "Time taken to build the pattern searcher: " << ms_double.count() << " ms" << std::endl;    
    }   
    std::ostringstream nullStream; // Pass a dummy stream
    
    cout << "Est_size: " << bit_s << endl;
    long long rbs = PS.bitsize();
    cout << "Bitsize:" << rbs << endl;
    cout << "bps: " << (float) rbs / txt_len << endl;

    vector<PatternResult> results;

    if (result.count("pattern_file")) {
        std::string pattern_file = result["pattern_file"].as<std::string>();
        std::ifstream file(pattern_file);
        if (!file) {
            std::cerr << "Error opening file: " << pattern_file << std::endl;
            return 1;
        }
        std::string pattern;
        std::vector<std::string> patterns;
        while (std::getline(file, pattern)) {
            patterns.push_back(pattern);
        }
        file.close();
        for (const auto& pattern : patterns) {
            std::cout << "Searching for pattern: \"" << pattern << "\"" << std::endl;
            std::vector<int> occurrences;
            auto t3 = high_resolution_clock::now();
            PS.search(&occurrences, pattern);
            auto t4 = high_resolution_clock::now();
            std::cout << "Occurrences Found: \t";
            sort(occurrences.begin(), occurrences.end());
            for (u_int i = 0; i < occurrences.size(); i++) {
                std::cout << occurrences[i] << " ";
            } std::cout << std::endl;
            results.push_back({pattern, (int)occurrences.size(), occurrences, 
                std::chrono::duration<double, std::milli>(t4 - t3).count()});
            if (TIME) {
                duration<double, std::milli> ms_double = t4 - t3;
                std::cout << "Time taken to search for the pattern: " << ms_double.count() << " ms" << std::endl;
            }
        }
        return 0;
    }    

    bool input_mode = true;
    while ( input_mode ) {
        string pattern;
        cout << "Enter the pattern to search (or leave empty and press ENTER): ";
        std::getline(std::cin, pattern);
        if (pattern.empty()) {
            cout << "Exiting." << endl;
            break;
        } 
        cout << "Searching for pattern: \"" << pattern << "\"" << endl;
        vector<int> occurrences;
        auto t3 = high_resolution_clock::now();
        PS.search(&occurrences, pattern);
        auto t4 = high_resolution_clock::now();
        cout << "Occurrences Found: \t";
        sort(occurrences.begin(), occurrences.end());
        for (u_int i = 0; i < occurrences.size(); i++) {
            cout << occurrences[i] << " ";
        } cout << endl;
        results.push_back({pattern, (int)occurrences.size(), occurrences, 
            std::chrono::duration<double, std::milli>(t4 - t3).count()});
        if (TIME) {
            duration<double, std::milli> ms_double = t4 - t3;
            std::cout << "Time taken to search for the pattern: " << ms_double.count() << " ms" << std::endl;
        }
    }

    std::string output_filename;
    if (result.count("output")) {
        output_filename = result["output"].as<std::string>();
        std::cout << "Output file: " << output_filename << std::endl;
    } else {
        std::cout << "Enter the output filename: ";
        std::cin >> output_filename;
    }
    std::ofstream out(output_filename);
    if (!out) {
        std::cerr << "Error opening output file: " << output_filename << std::endl;
        return 1;
    }
    out << "pattern occurrences time(ms) positions" << std::endl;
    for (const auto& result : results) {
        out << result.pattern << " " << result.occurrences << " " << result.time;
        for (const auto& pos : result.positions) {
            out << " " << pos;
        }
        out << std::endl;
    }
    out.close();

    return 0;
}

/*
    vector<string> patterns = {"b", "ab", "aab", "aaab", "aaaab", "aaaaab", "aaaaaab", "aaaaaaab", 
        "aaaaaaaab", "aaaaaaaaab", "aaaaaaaaaab", "aaaaaaaaaaab", "aaaaaaaaaaaab", "aaaaaaaaaaaaab",} ;
    
    for (string pattern : patterns) {        
        double times;
        for (int i = 0; i < 50; i++) {
            vector<int> occurrences;
            auto t3 = high_resolution_clock::now();
            PS.search(&occurrences, pattern);
            auto t4 = high_resolution_clock::now();
            duration<double, std::milli> ms_double = t4 - t3;
            times += ms_double.count();
        }
        double avg = times / 50;
        cout << "Average time for pattern " << pattern << ": " << avg << " ms" << endl;
    }
    return 0;


*/

/*
    vector<string> patterns = {"th", "he", "in", "er", "an", "re", "on", "at",
        "en", "nd", "ti", "es", "or", "te", "of", "ed", "is", "it", "al", "ar", 
        "st", "to", "nt", "ng", "se", "ha", "as", "ou", "io", "le", "ve", "co", 
        "me", "de", "hi", "ri", "ro", "ic", "ne", "ea", "ra", "ce", "li", "ch", 
        "ll", "be", "ma", "si", "om", "ur"};
    struct trio {
        int occs;
        int p_size;
        double time;
    };
    vector<trio> results;
    for (string pattern : patterns) {
        double times = 0;
        int occs;
        for (int i = 0; i < 50; i++) {
            vector<int> occurrences;
            auto t3 = high_resolution_clock::now();
            PS.search(&occurrences, pattern);
            auto t4 = high_resolution_clock::now();
            duration<double, std::milli> ms_double = t4 - t3;
            cout << ms_double.count() << " ";
            occs = occurrences.size();
            times += ms_double.count();
        }
        cout << endl;
        double avg = times / 50;
        results.push_back({occs, (int)pattern.size(), avg});
    }    
    ofstream out("results.txt");
    for (const auto& r : results) {
        out << r.occs << " " << r.p_size << " " << r.time << endl;
    }
    return 0;
*/
