#include <vector>
#include <iostream>
#include <cstdint>
#include <sdsl/bit_vectors.hpp>
#include "ppbv.hpp"

using namespace sdsl;
using namespace std;

typedef uint32_t u32; // this special trick will save us 1% code width bro trust me
typedef std::pair<u32, u32> Point;

#ifndef WAVELETMATRIX_HPP
#define WAVELETMATRIX_HPP

/// @brief A wavelet matrix using bit_vectors over an alphabet [1, sigma].
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
    WaveletMatrix();
    /// @brief Access the number in the i-th zero-indexed 
    /// position of the original sequence.
    /// @param i positive 0-indexed position.
    /// @return The number or NULL if out of bounds
    u32 access(u32 i);
    /// @brief Counts the occurences of number c up until yet excluding the given 
    /// zero-indexed position i
    /// @param i the zero-indexed position
    /// @param c the number
    /// @return the number of occurences until position i
    u32 rank(u32 c, u32 i);
    /// @brief Returns the 0-indexed position of the j-th occurence
    /// of the number c
    /// @param c a number
    /// @param j a positive number    
    /// @return the position or the size of the sequence if not found
    u32 select(u32 c, u32 j);
    void printself();
    ppbv operator[](u32 level);
    u32 offset(u32 level);
    u32 size() { return bm[0].size(); }
};

#endif