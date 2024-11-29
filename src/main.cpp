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
#include <cxxopts.hpp>

#include "patterns.hpp"

using namespace sdsl;
using namespace std;

int main(int argc, char* argv[]) {
    std::string input_filename;

    cxxopts::Options options("Pattern Searcher", "Search for text patterns in a text file");

    options.add_options()
        ("f,file", "Input file name", cxxopts::value<std::string>())
        ("d,debug", "Enable debugging mode")
        ("h,help", "Print usage");

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

    PatternSearcher PS(input_filename);

    while (true) {
        cout << "Enter the pattern to search: ";
        string pattern;
        getline(cin, pattern);
        cout << "Searching for pattern: \"" << pattern << "\"" << endl;
        vector<int> occurences;
        PS.search(&occurences, pattern);
        cout << "Occurences Found: \t";
        sort(occurences.begin(), occurences.end());
        for (u_int i = 0; i < occurences.size(); i++) {
            cout << occurences[i] << " ";
        } cout << endl;
    }

    return 0;
}