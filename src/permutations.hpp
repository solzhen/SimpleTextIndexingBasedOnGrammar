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

class Permutation {
    friend class PowerPermutation;
protected:
    Permutation();
public:
    int_vector<> pi; // permutation
    int_vector<> S; // shortcuts
    bit_vector b; // bit vector to mark shortcuts
    bit_vector::rank_1_type rank_b_support; // rank support for b
    int t; // parameter t    
    Permutation(int_vector<> pi, int t);
    int operator[](int i);
    int inverse(int i);
    int rank_b(int i);
};

class PowerPermutation: public Permutation {
public:
    bit_vector D; // bitvector D marks with 1 the end of a chunk (inclusive)
    bit_vector::rank_1_type rank_D_support;
    bit_vector::select_1_type select_D_support;
    Permutation tau; // permutation induced by the cycle decomposition
    PowerPermutation(int_vector<> pi, int t);
    int power(int i, int k);
    int rank_D(int i);
    int select_D(int i);
};

#endif // PERMUTATION_HPP