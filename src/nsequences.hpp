#include <sdsl/bit_vectors.hpp>
#include "permutations.hpp"
#include <iostream>
#include "helper.hpp"

using namespace sdsl;
using namespace std;

class RSequence {
private:
public:
    abv A_;
    rbv B_;
    vector<dbv> D;
    vector<Permutation> pi;
    int sigma; int n;
    int select_1_D(int k, int i);
    int select_0_D(int k, int i);
    int select_0_A(int k, int i);
    int select_1_A(int k, int i);
    int rank_A(int c, int i);
    int pred_0_A(int c, int s);

    RSequence();
    /// @brief Builds structure to support rank, select and access queries
    /// @param S integer vector representing the sequence
    /// @param sigma size of alphabet [0 . . . sigma)
    RSequence(int_vector<> S, int sigma);
    /// @brief Access query
    /// @param i position in the sequence
    /// @return The symbol at position i
    int access(int i); int operator[](int i) { return access(i); };
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
    // add printable
    void printself() {
        for (int i = 0; i < n; i++) {
            cout << access(i) << " ";
        } cout << endl;
    }
    long long bitsize() {
        return 0;
    }
};

void seq_test();