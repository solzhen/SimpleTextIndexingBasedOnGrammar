#include <iostream>
#include <cstdlib>

#ifndef PERMUTATION_HPP
#define PERMUTATION_HPP

#include <sdsl/bit_vectors.hpp>
#include <iostream>

using namespace sdsl;
using namespace std;

bool isPermutation(int_vector<> pi);

typedef struct drsv {
    bit_vector b;
    rank_support_v<1> rank; // rank_1
    select_support_mcl<1, 1> sel; // sel_1
} drsv;

typedef struct brv {
    bit_vector b;
    rank_support_v<1> rank; // rank_1
} brv;

/// @brief bitvector-based repersentation of a permutation using shortcuts
class Permutation {
    friend class PowerPermutation;
protected:    
    int rank_b(int i);
    int_vector<> pi; // permutation
    int_vector<> S; // shortcuts
    brv b; // bit vector to mark shortcuts
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
private:
    bool vectorset = false;
protected:
    int rank_D(int i);
    int select_D(int i);
    drsv D;
    Permutation tau; // permutation induced by the cycle decomposition
public:
    PowerPermutation(int_vector<> pi, int t);
    /// @brief Applies the permutation k times
    /// @param i 
    /// @param k 
    /// @return the position of the element i after applying the permutation k times
    int power(int i, int k);
};

void test_main();

#endif // PERMUTATION_HPP