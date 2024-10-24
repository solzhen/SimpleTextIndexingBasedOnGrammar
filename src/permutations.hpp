using namespace sdsl;
using namespace std;

#include <iostream>
#include <cstdlib>

#ifndef PERMUTATION_HPP
#define PERMUTATION_HPP

#include <sdsl/bit_vectors.hpp>
#include <iostream>

using namespace sdsl;
using namespace std;

bool isPermutation(int_vector<> pi);

/// @brief bitvector-based repersentation of a permutation using shortcuts
class Permutation {
    friend class PowerPermutation;
protected:    
    int rank_b(int i);
    int_vector<> pi; // permutation
    int_vector<> S; // shortcuts
    bit_vector b; // bit vector to mark shortcuts
    bit_vector::rank_1_type rank_b_support; // rank support for b
public:    
    Permutation();
    int t; // parameter t    
    
    /// @brief Sole constructor, it does not check if pi is a permutation
    /// @param pi vector of integers (initially, 8 bit long integers)
    /// @param t parameter t length of the shortcuts
    Permutation(int_vector<> pi, int t);

    /// @brief Return the position of the element i after applying the permutation
    int operator[](int i);
    int permute(int i) { return this->operator[](i); };

    /// @brief Return the inverse of the permutation, 
    /// that is, the position j such that permutation ( j ) = i
    /// @param i 
    /// @return 
    int inverse(int i);
};
/// @brief bitvector-based repersentation of a permutation using 
/// shortcuts with power operation support
class PowerPermutation: public Permutation {
    friend class PowerPermutationForSequences;
protected:
    int rank_D(int i);
    int select_D(int i);
    bit_vector D; // bitvector D marks with 1 the end of a chunk (inclusive)
    bit_vector::rank_1_type rank_D_support;
    bit_vector::select_1_type select_D_support;
    Permutation tau; // permutation induced by the cycle decomposition
public:
    PowerPermutation(int_vector<> pi, int t);
    /// @brief Return the position of the element i after applying the permutation k times
    /// @param i 
    /// @param k 
    /// @return 
    int power(int i, int k);
};

#endif // PERMUTATION_HPP