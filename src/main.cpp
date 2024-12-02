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
#include <sys/stat.h>
#include "patterns.hpp"

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

    string filename = input_filename;

    string command = "./repairs/repairnavarro/repair " + filename;
    int retCode = system(command.c_str());
    if (retCode == 0) {
        std::cout << "Program executed successfully!" << std::endl;
    } else {
        std::cout << "Program execution failed with code: " << retCode << std::endl;
    }

    ifstream Rfile(filename + ".R", ios::binary);
    if (!Rfile.is_open()) {
        cerr << "Error: cannot open file " << filename << ".R" << endl;
        return 1;
    }
    int alph;
    Rfile.read((char*)&alph, sizeof(int));
    cout << "Alphabet size: " << alph << endl;
    char map[256];
    Rfile.read((char*)&map, alph);
    cout << "Alphabet mapping: ";
    for (int i = 0; i < alph; i++) {
        cout << map[i] << " ";
    } cout << endl;
    vector<Tpair> rules;
    Tpair rule;
    while (Rfile.read((char*)&rule, sizeof(Tpair))) {
        rules.push_back(rule);
    }
    cout << "Number of rules: " << rules.size() << endl;

    ifstream Cfile(filename + ".C", ios::binary);
    if (!Cfile.is_open()) {
        cerr << "Error: cannot open file " << filename << ".C" << endl;
        return 1;
    }

    int len;
    struct stat s;
    char fname[1024];
    strcpy(fname, filename.c_str());
    strcat(fname,".C");
    stat (fname,&s);
    len = s.st_size / sizeof(int);

    int* C = (int*)malloc(len * sizeof(int));
    Cfile.read((char*)C, len * sizeof(int));
    Cfile.close();
    cout << "Sequence C length: " << len << endl;
    cout << "Sequence C: ";
    for (int i = 0; i < len; i++) {
        cout << C[i] << " ";
    } cout << endl;
    Cfile.close();

    int nnt = rules.size();

    unordered_map<int, string> memo;

    for (int i = 0; i < len; i++) {
        if (C[i] - alph > nnt) {
            break;
        }
        if (C[i] < alph) {
            cout << map[C[i]];
        } else {
            cout << expandRule(rules.data(), C[i]-alph, memo, alph, map);
        }
    } cout << endl;

    return 0;    

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