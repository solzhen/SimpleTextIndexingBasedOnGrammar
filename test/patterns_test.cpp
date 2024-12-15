#include <chrono>
#include <catch2/catch_test_macros.hpp>
#include "patterns.hpp"
#include "globals.h"
#include <bits/this_thread_sleep.h>

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

    REQUIRE_FALSE(g_fileName.empty());

    string input_filename = g_fileName;
    string output_filename = g_fileName + ".results.txt";  
    

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    TIME = true;

    u_int text_length;
    u_int num_rules;

    //DEBUG = true;

    auto t1 = high_resolution_clock::now();
    PatternSearcher PS = PatternSearcher(input_filename, &text_length, &num_rules);
    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double;
    if (TIME) {
        ms_double = t2 - t1;
        std::cout << "Time taken to build the pattern searcher: " << ms_double.count() << " ms" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    string filecontent = "";
    // Open the file in input mode
    std::ifstream file(input_filename, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file!" << std::endl;
    }
    std::ostringstream content;
    content << file.rdbuf();
    filecontent = content.str();
    file.close();

    cout << filecontent.size() << endl;   

    struct tuple {
        int occs;
        int p_size;
        double ps_time;
        double std_time;
    };
    vector <tuple> results;
    ofstream out(output_filename);
    if (!out) {
        std::cerr << "Error opening file!" << std::endl;
    }
    if (TIME) {
        out << text_length << " " << num_rules << " " << ms_double.count() << endl;
    }    

    vector<int> ms = {5, 10, 20, 30, 40, 50, 70, 100} ;    

    for (int m : ms) {   
        results.clear();
        for (int i = 0; i < 100; i++) {
            int r = rand() % (filecontent.size() - m - 1);
            std::string pattern = filecontent.substr(r, m);
            //cout << i << ": Searching for pattern: \"" << pattern << "\"" << endl;
            vector<int> occurences;
            //pattern = asciiContent.substr(96 - m, m);
            //pattern = "GGGG";            
            auto t3 = high_resolution_clock::now();
            PS.search(&occurences, pattern);
            auto t4 = high_resolution_clock::now();
            sort(occurences.begin(), occurences.end());
            auto t5 = high_resolution_clock::now();
            vector<int> expected_occurences = findOccurrences(filecontent, pattern);
            auto t6 = high_resolution_clock::now();
            REQUIRE(occurences == expected_occurences);
            if (TIME) {
                duration<double, std::milli> ms_double = t4 - t3;
                duration<double, std::milli> ms_double2 = t6 - t5;
                results.push_back({(int)occurences.size(), (int)pattern.size(), ms_double.count(), ms_double2.count()});
            }
        }
        if (TIME) {
            cout << "Pattern size: " << m << endl;
            double sum_ps = 0;
            double sum_std = 0;
            for (const auto& r : results) {
                sum_ps += r.ps_time;
                sum_std += r.std_time;
            }
            double avg_ps = sum_ps / results.size();
            double avg_std = sum_std / results.size();
            std::cout << "Average time by PS: " << avg_ps << " ms" << std::endl;            
            std::cout << "Average time by std: " << avg_std << " ms" << std::endl;
        }
        if (TIME) {        
            for (const auto& r : results) {
                out << r.occs << " " << r.p_size << " " << r.ps_time << " " << r.std_time << endl;
            } 
        }
    }
    out.close();
}

TEST_CASE("PatternSearcherBig","[big]") {   
    REQUIRE_FALSE(g_fileName.empty());
    string input_filename = g_fileName;
    string output_filename = g_fileName + ".big.results.txt";
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    TIME = true;
    u_int text_length;
    u_int num_rules;
    auto t1 = high_resolution_clock::now();
    PatternSearcher PS = PatternSearcher(input_filename, &text_length, &num_rules);
    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double;
    if (TIME) {
        ms_double = t2 - t1;
        std::cout << "Time taken to build the pattern searcher: " << ms_double.count() << " ms" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    string filecontent = "";
    // Open the file in input mode
    std::ifstream file(input_filename, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "Error opening input file!" << std::endl;
    }
    std::ostringstream content;
    content << file.rdbuf();
    filecontent = content.str();
    file.close();
    struct triple {
        int occs;
        int p_size;
        double ps_time;
    };
    vector <triple> results;
    ofstream out(output_filename);
    if (!out) {
        std::cerr << "Error opening output file!" << std::endl;
    }
    if (TIME) {
        out << text_length << " " << num_rules << " " << ms_double.count() << endl;
    }    
    vector<int> ms = {5, 10, 20, 30, 40, 50, 70, 100};
    int n = 100;
    for (int m : ms) {   
        results.clear();
        for (int i = 0; i < n; i++) {
            int r = rand() % (filecontent.size() - m - 1);
            std::string pattern = filecontent.substr(r, m);
            vector<int> occurences;
            auto t3 = high_resolution_clock::now();
            PS.search(&occurences, pattern);
            auto t4 = high_resolution_clock::now();
            if (TIME) {
                duration<double, std::milli> ms_double = t4 - t3;
                results.push_back({(int)occurences.size(), (int)pattern.size(), ms_double.count()});
            }
            cout << "\r" << i+1 << "/" << n  << flush; 
        } cout << "\r" << n << "/" << n << endl;
        if (TIME) {
            cout << "Pattern size: " << m << endl;
            double sum_ps = 0;
            for (const auto& r : results) {
                sum_ps += r.ps_time;
                out << r.occs << " " << r.p_size << " " << r.ps_time << endl;
            }
            double avg_ps = sum_ps / results.size();
            std::cout << "Average time by PS: " << avg_ps << " ms" << std::endl;
        }
    }
    out.close();
}

TEST_CASE("PatternSearcherConstructors","[constructors]") {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    TIME = true;
    int n = 2;
    vector<string> filenames = {
        "textfile/bill.txt", 
        "textfile/scream.txt", 
        "textfile/fall_of_house_usher.txt", 
        "textfile/the_raven.txt",
        "textfile/poetics.txt",
        "textfile/alice_s_adventures_in_wonderland.txt",
        "textfile/emily_dickson_poems.txt",
        "textfile/pinocchio.txt",
        "textfile/eureka.txt",
        "textfile/apology_crito_phaedo.txt",
        "textfile/cuentos_chilenos.txt",
        "textfile/the_republic.txt",
        "textfile/quijote.txt",
    };
    ofstream out("construction.results.txt");
    if (!out) {
        std::cerr << "Error opening file!" << std::endl;
        exit(1);
    }
    for (const auto& filename : filenames) {
        u_int text_length;
        u_int num_rules;        
        double sum1 = 0;
        for (int i = 0; i < n; i ++) {
            auto t1 = high_resolution_clock::now();
            PatternSearcher PS = PatternSearcher(filename, &text_length, &num_rules);
            auto t2 = high_resolution_clock::now();
            duration<double, std::milli> ms_double = t2 - t1;
            sum1 += ms_double.count();
            //cout << "Time taken to build the pattern searcher: " << ms_double.count() << " ms" << endl;
        }
        double avg1 = sum1 / n; 
        std::cout << "Average time to build the pattern searcher: " << avg1 << " ms" << std::endl;
        cout << text_length << " " << num_rules << " " << avg1 << endl;
        out << filename << " " << text_length << " " << num_rules << " " << avg1 << endl;
    }
}


TEST_CASE("PatternSearcher_aaab","[aaab]") {    

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    TIME = true;

    u_int text_length;
    u_int num_rules;

    string input_filename = "textfile/aaaaaaaaab.txt";
    string output_filename = "aaaaaaaaab.fixed_occ.txt";

    PatternSearcher PS = PatternSearcher(input_filename, &text_length, &num_rules);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    string filecontent = "";
    // Open the file in input mode
    std::ifstream file(input_filename, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file!" << std::endl;
    }
    struct tuple {
        int occs;
        double ps_time;
    };
    vector <tuple> results;
    ofstream out(output_filename);
    if (!out) {
        std::cerr << "Error opening file!" << std::endl;
    }
    vector<int> ms = {1, 5, 10, 20, 30, 40, 50, 70};

    vector<string> patterns = {
        "b\n", 
        "ab\n", 
        "aaab\n", 
        "aaaaab\n", 
        "aaaaaaab\n", 
        "aaaaaaaaaaab\n", 
        "aaaaaaaaaaaaaab\n",
        "aaaaaaaaaaaaaaaab\n",
        "baaaaaaaaaaaaaaaaaab\n", 
        "aabaaaaaaaaaaaaaaaaaab\n", 
        "aaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaaaaaaabaaaaaaaaaaaaaaaaaab\n", 
        "aaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "baaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "baaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "baaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
        "aaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaab\n",
    };

    int n = 100;

    for (string pattern : patterns) {   
        double sum_times = 0;
        for (int i = 0; i < n; i++) {
            vector<int> occurences;
            auto t3 = high_resolution_clock::now();
            PS.search(&occurences, pattern);
            auto t4 = high_resolution_clock::now();
            if (TIME) {
                cout << "occs: " << occurences.size() << " " << flush;
                duration<double, std::milli> ms_double = t4 - t3;
                sum_times += ms_double.count();
            } 
        } cout << endl;
        if (TIME) {
            double avg = sum_times / n;
            std::cout << "Pattern size: " << pattern.size() << endl;
            std::cout << "Average time by PS: " << avg << " ms" << std::endl;
            out << pattern.size() << " " << avg << endl;
        }
    }
    out.close();
}

TEST_CASE("2-size Pattern Search","[2size]") {
    string output_filename = "2m.results.txt";   

    vector<string> filenames = {
        "textfile/bill.txt", 
        "textfile/fall_of_house_usher.txt", 
        "textfile/the_raven.txt",
        "textfile/poetics.txt",
        "textfile/alice_s_adventures_in_wonderland.txt",
        "textfile/emily_dickson_poems.txt",
        "textfile/pinocchio.txt",
        "textfile/eureka.txt",
        "textfile/apology_crito_phaedo.txt",
        "textfile/the_republic.txt",
    };   

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    vector<string> patterns = {"th", "he", "in", "er", "an", "re", "on", "at",
        "en", "nd", "ti", "es", "or", "te", "of", "ed", "is", "it", "al", "ar", 
        "st", "to", "nt", "ng", "se", "ha", "as", "ou", "io", "le", "ve", "co", 
        "me", "de", "hi", "ri", "ro", "ic", "ne", "ea", "ra", "ce", "li", "ch", 
        "ll", "be", "ma", "si", "om", "ur"};
    struct trio {
        string pattern;
        u_int txt_len;
        int occs;
        double time;
    };
    ofstream out(output_filename);
    for (string filename: filenames) {
        u_int text_length;
        u_int num_rules;
        PatternSearcher PS = PatternSearcher(filename, &text_length, &num_rules);
        vector<trio> results;
        for (string pattern : patterns) {
            double times = 0;
            int occs;
            for (int i = 0; i < 50; i++) {
                vector<int> occurences;
                auto t3 = high_resolution_clock::now();
                PS.search(&occurences, pattern);
                auto t4 = high_resolution_clock::now();
                duration<double, std::milli> ms_double = t4 - t3;
                occs = occurences.size();
                times += ms_double.count();
            }
            cout << "." << flush;
            double avg = times / 50;
            results.push_back({pattern, text_length, occs, avg});
        }    
        for (const auto& r : results) {
            out << text_length << " " << r.pattern << " " << r.occs << " " << r.time << endl;
        }
    }


}