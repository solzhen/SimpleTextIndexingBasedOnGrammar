#include <catch2/catch_test_macros.hpp>
#include "patterns.hpp"

vector<int> findOccurrences(const string& filecontent, 
    const string& pattern) {
    vector<int> occurrences;
    for (size_t i = 0; i < filecontent.size(); i++) {
        if (filecontent.substr(i, pattern.length()) == pattern) {
            occurrences.push_back(i);
        }
    }
    return occurrences;
}

TEST_CASE("PatternSearcher for poema20.txt") {
    string input_filename = "textfile/poema20.txt";
    FILE *input  = fopen(input_filename.c_str(), "rb");
    string filecontent = "";
    char c;
    while (fread(&c, 1, 1, input) == 1) {
        filecontent += c;
    }
    fclose(input);

    FILE* originalStdout = stdout;
    stdout = fopen("/dev/null", "w");
    streambuf* originalCoutBuffer = cout.rdbuf();
    stringstream buffer;
    cout.rdbuf(buffer.rdbuf());    

    PatternSearcher PS(input_filename);

    stdout = originalStdout;
    cout.rdbuf(originalCoutBuffer);
    fflush(stdout);    
    
    cout << PS.numRules() << " rules" << endl;  

    for (int i = 0; i < 10; i++) {
        string pattern = filecontent.substr(rand() % (filecontent.size() - 10), rand() % 10 + 1);
        cout << "Searching for pattern: \"" << pattern << "\"" << endl;
        vector<int> occurences;
        PS.search(&occurences, pattern);
        sort(occurences.begin(), occurences.end());
        vector<int> expected_occurences = findOccurrences(filecontent, pattern);
        REQUIRE(occurences == expected_occurences);
    }    
}