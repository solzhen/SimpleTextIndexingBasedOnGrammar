#include <sdsl/bit_vectors.hpp>
#include "permutations.hpp"
#include <iostream>

using namespace sdsl;
using namespace std;

typedef struct abv {
    bit_vector b;
    rank_support_v<0> rank;
    select_support_mcl<1, 1> sel_1;
    select_support_mcl<0, 1> sel_0;
} abv;

typedef struct dbv {
    bit_vector b;
    select_support_mcl<1, 1> sel_1;
    select_support_mcl<0, 1> sel_0;
} dbv;

class ARSSequence {
private:
    vector<abv> A;
    vector<dbv> D;
    vector<Permutation> pi;
    int sigma; int n;
    int select_1_D(int k, int i);
    int select_0_D(int k, int i);
    int select_0_A(int k, int i);
    int select_1_A(int k, int i);
    int rank_A(int c, int i);
    int pred_0_A(int c, int s);
public:
    /// @brief Builds structure to support rank, select and access queries
    /// @param S integer vector representing the sequence
    /// @param sigma size of alphabet [0 . . . sigma)
    ARSSequence(int_vector<> S, int sigma);
    /// @brief Access query
    /// @param i position in the sequence
    /// @return The symbol at position i
    int access(int i);
    int operator[](int i) { return access(i); };
    /// @brief Rank query
    /// @param c symbol in the alphabet
    /// @param i position in the sequence
    /// @return The number of occurrences of c in the sequence up to and including position i    
    int rank(int c, int i);
    /// @brief Select query
    /// @param c symbol in the alphabet 
    /// @param i the i-th occurrence of c in the sequence
    /// @return The position of the i-th occurrence of c in the sequence
    /// @note Position returned is 0-indexed, while parameter i is 1-indexed as ordinal numbers are.
    int select(int c, int j);
    u_int size() { return n; }
};

void seq_test();