#include <vector>
#include <iostream>
#include <cstdint>
#include <sdsl/bit_vectors.hpp>

using namespace sdsl;
using namespace std;

typedef uint32_t u32; // this special trick will save us 1% code width bro trust me
typedef std::pair<u32, u32> Point;

#ifndef WAVELETMATRIX_HPP
#define WAVELETMATRIX_HPP

class preprocessed_bitvector {
private:     
    rrr_vector<127> rrrb;
    rrr_vector<127>::rank_1_type rank_1_;
    rrr_vector<127>::rank_0_type rank_0_;
    rrr_vector<127>::select_1_type select_1_;
    rrr_vector<127>::select_0_type select_0_;
    bit_vector b;
public:    
    preprocessed_bitvector(bit_vector &bv);
    void preprocess();
    u32 rank_1(u32 i);
    u32 rank_0(u32 i);
    u32 select_1(u32 i);
    u32 select_0(u32 i);
    void printself(u32 l, u32 z);
    void printself();
    u32 operator[](u32 i);
};
using ppbv = preprocessed_bitvector;
class WaveletMatrix {
private:    
    u32 sigma; // highest symbol in the alphabet
    vector<u32> z; // right child pointer
    void build(vector<u32>& S, u32 n, u32 sigma);
    u32 select(u32 l, u32 p, u32 a, u32 b, u32 c, u32 j);
    vector<ppbv> bm; // bit matrix seen as vector of preprocessed bit vectors
public:    
    /// @brief A wavelet matrix using bit_vectors over an alphabet [1, sigma]
    /// @param s 4-byte long unsigned integer vector
    /// @param sigma highest numerical symbol
    WaveletMatrix(vector<u32>& s, u32 sigma);
    /// @brief Access the number in the i-th zero-indexed 
    /// position of the original sequence.
    /// @param i positive 0-indexed position.
    /// @return The number or NULL if out of bounds
    u32 access(u32 i);
    /// @brief Counts the occurences of number c up until the given 
    /// zero-indexed position
    /// @param i the zero-indexed position
    /// @param c the number (may not be in sequence)
    /// @return the number of occurences
    u32 rank(u32 i, u32 c);
    /// @brief Returns the zero-indexed position of the j-th occurence
    /// of the number c
    /// @param j a positive number
    /// @param c a number
    /// @return the position or the size of the sequence if not found
    u32 select(u32 j, u32 c);
    void printself();
};

#endif