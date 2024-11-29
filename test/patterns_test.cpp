#include <catch2/catch_test_macros.hpp>
#include "patterns.hpp"

#define REDIRECT_STDOUT \
    FILE* originalStdout = stdout; \
    stdout = fopen("/dev/null", "w"); \
    streambuf* originalCoutBuffer = cout.rdbuf(); \
    stringstream buffer; \
    cout.rdbuf(buffer.rdbuf());

#define RESTORE_STDOUT \
    stdout = originalStdout; \
    cout.rdbuf(originalCoutBuffer); \
    fflush(stdout);

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

TEST_CASE("PatternSearcher","[pattern]") {
    string input_filename = "textfile/quijote.txt";
    FILE *input  = fopen(input_filename.c_str(), "rb");
    string filecontent = "";
    char c;
    while (fread(&c, 1, 1, input) == 1) {
        filecontent += c;
    }
    fclose(input);

    PatternSearcher PS(input_filename);

    unordered_map<int, string> memo;

    vector<int> occs;
    PS.search(&occs, "i d");
    sort(occs.begin(), occs.end());
    

    vector<int> expected_occurences = findOccurrences(filecontent, "i d");
    REQUIRE(occs == expected_occurences);


    //DEBUG = true;

    for (int i = 0; i < 50; i++) {
        string pattern = filecontent.substr(rand() % (filecontent.size() - 10), rand() % 10 + 1);
        cout << i << ": Searching for pattern: \"" << pattern << "\"" << endl;
        vector<int> occurences;
        PS.search(&occurences, pattern);
        sort(occurences.begin(), occurences.end());
        vector<int> expected_occurences = findOccurrences(filecontent, pattern);
        REQUIRE(occurences == expected_occurences);
    }    
}