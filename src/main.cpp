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

#include "patterns.hpp"

using namespace sdsl;
using namespace std;

int main(int argc, char* argv[]) {
    std::string input_filename;
    if (argc < 2) {
        std::cout << "Enter the input filename: ";
        std::cin >> input_filename;
    }
    else {
        input_filename = argv[1];
    }
    PatternSearcher PS(input_filename);

    FILE *input  = fopen(input_filename.c_str(), "rb");
    string filecontent = "";
    char c;
    while (fread(&c, 1, 1, input) == 1) {
        filecontent += c;
    }
    fclose(input);
    cout << "File content: " << filecontent << endl;


    while (true) {
        cout << "Enter the pattern to search: ";
        string pattern;
        cin >> pattern;
        vector<int> occurences;
        PS.search(&occurences, pattern);
        cout << "Occurences Found: \t";
        sort(occurences.begin(), occurences.end());
        for (u_int i = 0; i < occurences.size(); i++) {
            cout << occurences[i] << " ";
        } cout << endl;
        cout << "Expected Occurences: \t";
        for (u_int i = 0; i < filecontent.size(); i++) {
            if (filecontent.substr(i, pattern.length()) == pattern) {
                cout << i << " ";
            }
        } cout << endl;
    }


    return 0;
    /*
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
    return 0;*/
}