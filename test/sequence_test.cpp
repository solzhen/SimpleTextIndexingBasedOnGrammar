#include <catch2/catch_test_macros.hpp>
#include "sequences.hpp"
#include "nsequences.hpp"

// count the number of occurrences of symbol c in v up to and including position i
int t_rank(int_vector<> v, int c, int i) {
    int count = 0;
    for (int j = 0; j <= i; j++) {
        if (v[j] == c) count++;
    }
    return count;
}
// return the position of the j-th occurrence of symbol c in v
int t_select(int_vector<> v, int c, int j) {
    if (j == 0) return -1;
    int count = 0;
    for (int i = 0; i < v.size(); i++) {
        if (v[i] == c) {
            count++;
            if (count == j) return i;
        }
    }
    return v.size();
}

TEST_CASE("Sequence basic operations on 1,0 alternating sequence", "[sequence]") {
    int_vector<> vec({1, 0, 2, 1, 2, 2, 2, 0, 1, 0, 0, 3, 2, 1, 0, 0, 2, 3, 5, 4, 7, 0, 1, 2, 4, 4, 6, 0, 0, 1, 1, 0});
    ARSSequence seq = ARSSequence(vec, 8);
    // print vector
    cout << "S = " << vec << endl;    
    // print member A
    for (u_int i = 0; i < seq.A.size(); i++) {
        cout << "A[" << i << "].b: " << seq.A[i].b << endl;
    }
    // print member D
    for (u_int i = 0; i < seq.D.size(); i++) {
        cout << "D[" << i << "].b: " << seq.D[i].b << endl;
    }
    // print member pi
    for (u_int i = 0; i < seq.pi.size(); i++) {
        cout << "pi[" << i << "]: " << seq.pi[i].pi << endl;
    }
    cout << "Accessing sequence..." << endl;
    for (u_int i = 0; i < vec.size(); i++) {
        REQUIRE(seq.access(i) == vec[i]);
    }
    cout << "Rank queries..." << endl;
    // rank(c,i) for position i the symbol c: the number of c's in the sequence up to and including position i
    for (u_int i = 0; i < vec.size(); i++) {
        for (u_int c = 0; c < 8; c++) {
            REQUIRE(seq.rank(c, i) == t_rank(vec, c, i));
        }
    }
    cout << "Select queries..." << endl;
    // select(c,j) for the j-th occurrence of symbol c in the sequence return its position
    for (u_int j = 1; j < vec.size(); j++) {
        for (u_int c = 0; c < 8; c++) {
            REQUIRE(seq.select(c, j) == t_select(vec, c, j));
        }
    }
    
}

TEST_CASE("New Sequence basic operations on 1,0 alternating sequence", "[sequence]") {
    int_vector<> vec({1, 0, 2, 1, 2, 2, 2, 0, 1, 0, 0, 3, 2, 1, 0, 0, 2, 3, 5, 4, 7, 0, 1, 2, 4, 4, 6, 0, 0, 1, 1, 0});
    RSequence seq = RSequence(vec, 8);
    // print vector
    cout << "S = " << vec << endl;    
    // print member A_
    cout << "A_.b: " << seq.A_.b << endl;
    // print member B_
    cout << "B_.b: " << seq.B_.b << endl;
    // print member D
    for (u_int i = 0; i < seq.D.size(); i++) {
        cout << "D[" << i << "].b: " << seq.D[i].b << endl;
    }
    // print member pi
    for (u_int i = 0; i < seq.pi.size(); i++) {
        cout << "pi[" << i << "]: " << seq.pi[i].pi << endl;
    }
    cout << "Accessing sequence..." << endl;
    for (u_int i = 0; i < vec.size(); i++) {
        REQUIRE(seq.access(i) == vec[i]);
    }
    cout << "Rank queries..." << endl;
    for (u_int i = 0; i < vec.size(); i++) {
        for (u_int c = 0; c < 8; c++) {
            REQUIRE(seq.rank(c, i) == t_rank(vec, c, i));
        }
    }
    cout << "Select queries..." << endl;
    for (u_int j = 1; j < vec.size(); j++) {
        for (u_int c = 0; c < 8; c++) {
            REQUIRE(seq.select(c, j) == t_select(vec, c, j));
        }
    }
}


/*
    REQUIRE(seq.rank(0, 0) == 0);
    REQUIRE(seq.rank(0, 1) == 1);
    REQUIRE(seq.rank(0, 4) == 1);
    REQUIRE(seq.rank(0, 7) == 2);
    REQUIRE(seq.rank(0, 9) == 3);
    REQUIRE(seq.rank(0, 10) == 4);
    REQUIRE(seq.rank(0, 11) == 4);
    REQUIRE(seq.rank(0, 13) == 4);
    REQUIRE(seq.rank(0, 14) == 5);
    REQUIRE(seq.rank(0, 15) == 6);
    REQUIRE(seq.rank(0, 16) == 6);
    REQUIRE(seq.rank(0, 20) == 6);
    REQUIRE(seq.rank(0, 21) == 7);
    REQUIRE(seq.rank(0, 27) == 8);
    REQUIRE(seq.rank(0, 28) == 9);
    REQUIRE(seq.rank(0, 31) == 10);
    REQUIRE(seq.rank(1, 0) == 1);
    REQUIRE(seq.rank(1, 1) == 1);
    REQUIRE(seq.rank(1, 3) == 2);
    REQUIRE(seq.rank(1, 8) == 3);
    REQUIRE(seq.rank(1, 13) == 4);
    REQUIRE(seq.rank(1, 22) == 5);
    REQUIRE(seq.rank(1, 29) == 6);
    REQUIRE(seq.rank(1, 30) == 7);
    REQUIRE(seq.rank(1, 31) == 7);
    REQUIRE(seq.rank(2, 0) == 0);
    REQUIRE(seq.rank(2, 2) == 1);
    REQUIRE(seq.rank(2, 4) == 2);
    REQUIRE(seq.rank(2, 5) == 3);
    REQUIRE(seq.rank(2, 6) == 4);
    REQUIRE(seq.rank(2, 7) == 4);
    REQUIRE(seq.rank(2, 12) == 5);*/