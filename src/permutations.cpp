#include <sdsl/bit_vectors.hpp>

#include <iostream>

using namespace sdsl;
using namespace std;

#include <iostream>
#include <cstdlib>

#include "permutations.hpp"

#define ASSERT_EQUAL(expr1, expr2) \
    if ((expr1) != (expr2)) { \
        std::cerr << "Assertion failed: (" << #expr1 << " == " << #expr2 << ") evaluated as (" << (expr1) << " != " << (expr2) << ")\n\t" << __FILE__ << ", line " << __LINE__ << std::endl; \
        std::abort(); \
    } else { \
        std::cout << "Success: " << #expr1 << " : " << (expr1) << "  == " << (expr2) << ",\n\t" << __FILE__ << ", line " << __LINE__ << std::endl; \
    }

class Permutation {
public:
    vector<uint16_t> pi; // permutation
    vector<uint16_t> S; // shortcuts
    bit_vector b; // bit vector to mark shortcuts
    bit_vector::rank_1_type rank_b_support; // rank support for b
    int t; // parameter t
    Permutation() {};
    Permutation(vector<uint16_t> pi, int t) {        
        int n = pi.size();
        bit_vector v(n, 0);
        bit_vector b(n, 0);
        for (int i = 0; i < n; i++) {
            if (v[i] == 0) {
                v[i] = 1; 
                int j = pi[i]; int k = 1;
                while (j != i) {
                    if (k % t == 0) {
                        b[j] = 1; 
                    }
                    v[j] = 1; 
                    j = pi[j]; k++;
                }
                if (k > t) {
                    b[i] = 1;
                }            
            }
        }
        this->b = b;
        bit_vector::rank_1_type b_rank(&b);
        int rank = b_rank(n);
        vector<uint16_t> shortcuts(rank);
        for (int i = 0; i <= n; i++) {
            if (v[i] == 1) { 
                v[i] = 0; int j = pi[i]; 
                while (v[j] == 1) {
                    if (b[j] == 1) { 
                        shortcuts[b_rank(j + 1) - 1] = i; 
                        i = j; 
                    }                   
                    v[j] = 0; j = pi[j];                
                }
                if (b[j] == 1) {
                    shortcuts[b_rank(j + 1) - 1] = i;
                }
                i = j;
            }
        }        
        this->pi = pi;
        this->S = shortcuts;
        this->rank_b_support = b_rank;
        this->t = t;
    };
    int operator[](int i) {
        return pi[i];
    }
    int inverse(int i) {
        rank_b_support.set_vector(&b);
        if (i >= this->pi.size()) {
            return -1;
        }
        int j = i; bool s = true;
        int loop=0;
        while (pi[j] != i) {
            loop++;
            if (s && b[j] == 1) {
                s = false;
                j = S[rank_b_support(j)];
                //cout << "j: " << j << endl;
            } else {
                j = pi[j];
            }
        }
        return j;
    }
    // return number of 1s in b[0..i]
    int rank_b(int i) {
        return rank_b_support.rank(i+1);
    }
};

class PowerPermutation: public Permutation {
public:
    bit_vector D; // bitvector D marks with 1 the end of a chunk (inclusive)
    bit_vector::rank_1_type rank_D_support;
    bit_vector::select_1_type select_D_support;
    Permutation tau; // permutation induced by the cycle decomposition
    PowerPermutation(vector<uint16_t> pi, int t): Permutation(pi, t) {
        int n = pi.size();
        bit_vector v(n, 0);
        vector<uint16_t> tau_v(n); 
        bit_vector  D(n); 
        int tau_i = 0;
        for (int i = 0; i < n; i++) {
            if (v[i] == 0) {
                v[i] = 1; 
                int j = pi[i]; int k = 1;
                tau_v[tau_i] = i; D[tau_i] = 0; tau_i++;
                while (j != i) {
                    tau_v[tau_i] = j; D[tau_i] = 0; tau_i++;
                    v[j] = 1; 
                    j = pi[j]; k++;                
                }
                D[tau_i - 1] = 1;
            }
        }
        this->tau = Permutation(tau_v, t);
        this->D = D;
        this->rank_D_support = bit_vector::rank_1_type(&D);
        this->select_D_support = bit_vector::select_1_type(&D);
        // call the constructor of the base class
    }
    // return the k-th power of pi[i]
    int power(int i, int k) {
        select_D_support.set_vector(&D);
        rank_D_support.set_vector(&D);
        int j = tau.inverse(i);
        int chunk_number = this->rank_D(j - 1); //[0..]
        int pred = this->select_D(chunk_number) + 1; // [0..]
        int succ = this->select_D(chunk_number + 1);
        return tau[ pred + ( (j-pred + k) % (succ - pred + 1) ) ];
    }
    // return number of 1s in D[0..i]
    int rank_D(int i) {
        return this->rank_D_support.rank(i+1); //sdsl rank return [0::i) exclusive i
    }
    // return the position of the i-th 1 in D
    int select_D(int i) {
        select_D_support.set_vector(&D);
        if (i==0) return -1;
        return this->select_D_support.select(i);
    }
};

void test_main() {    
    vector<uint16_t> pi2 = {1, 2, 3, 4, 0};
    vector<uint16_t> pi3 = {4, 3, 2, 1, 0};
    Permutation pi2i = Permutation(pi2, 2);
    ASSERT_EQUAL(pi2i.pi, pi2);
    ASSERT_EQUAL(pi2i.inverse(0), 4);
    ASSERT_EQUAL(pi2i.inverse(1), 0);
    Permutation pi3i = Permutation(pi3, 3);
    ASSERT_EQUAL(pi3i.inverse(0), 4);
    ASSERT_EQUAL(pi3i.inverse(1), 3);
    ASSERT_EQUAL(pi3i.inverse(2), 2);
    vector<uint16_t> pi1 = {9, 6, 2, 4, 7, 0, 10, 11, 3, 5, 8, 1, 12};
    Permutation pi1i = Permutation(pi1, 3);
    ASSERT_EQUAL(pi1i.inverse(0), 5); 
    ASSERT_EQUAL(pi1i.inverse(1), 11);
    ASSERT_EQUAL(pi1i.inverse(2), 2);
    ASSERT_EQUAL(pi1i.inverse(3), 8);
    ASSERT_EQUAL(pi1i.inverse(1), 11);
    vector<uint16_t> ppp = {9, 6, 2, 4, 7, 0, 10, 11, 3, 5, 8, 1};
    PowerPermutation power_perm = PowerPermutation(ppp, 3);
    ASSERT_EQUAL(power_perm.tau.pi, vector<uint16_t>({0,9,5,1,6,10,8,3,4,7,11,2}));
    ASSERT_EQUAL(power_perm.D, bit_vector({0,0,1,0,0,0,0,0,0,0,1,1}));
    ASSERT_EQUAL(power_perm.inverse(0), 5);
    ASSERT_EQUAL(power_perm.inverse(1), 11);
    ASSERT_EQUAL(power_perm.inverse(3), 8);
    ASSERT_EQUAL(power_perm.inverse(4), 3);
    ASSERT_EQUAL(power_perm.inverse(5), 9);
    ASSERT_EQUAL(power_perm.inverse(2), 2);
    ASSERT_EQUAL(power_perm.power(0, 1), 9);
    ASSERT_EQUAL(power_perm.power(0, 2), 5);
    ASSERT_EQUAL(power_perm.power(0, 3), 0);
    ASSERT_EQUAL(power_perm.power(1, 1), 6);
    ASSERT_EQUAL(power_perm.power(1, 6), 7);
    ASSERT_EQUAL(power_perm.power(11, 8), 11);
    ASSERT_EQUAL(power_perm.power(2, 7), 2);
    cout << "All tests passed!" << endl;
}

/*
int main() {
    test_main();
    return 0;
}
*/





// g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib permutations.cpp -o permutations.o -lsdsl -ldivsufsort -ldivsufsort64


/*
struct permutation {
    vector<uint16_t> pi; // permutation
    vector<uint16_t> S; // shortcuts
    bit_vector b; // bit vector to mark shortcuts
    rank_support_v<1> rank; // rank support for b
    int t; // parameter t
    vector<uint16_t> tau; // permutation induced by the cycle decomposition
    vector<uint16_t> D; // bitvector D
};

struct permutation construct(vector<uint16_t> pi, int t) {
    int n = pi.size();
    // bit vector to mark visited elements
    bit_vector v(n, 0); 
    // bit vector to mark the shortcuts, where B[i]==1 iff there is a shortcut
    // leaving from i
    bit_vector b(n, 0); 


    // let C1, C2, ... Cc be the cycle decomposition of pi, and l_i = |Ci|
    // let tau = C1 C2 ... Cc be the concatenation of the cycles 
    // and a bitvector D = 0^(l_1-1) 1 0^(l_2-1) 1 ... 0^(l_c-1) 1
    vector<uint16_t> tau(n); // permutation induced by the cycle decomposition
    vector<uint16_t> D(n); // bitvector D

    int tau_i = 0;

    for (int i = 0; i < n; i++) {
        if (v[i] == 0) {
            v[i] = 1; 
            int j = pi[i]; int k = 1;
            tau[tau_i] = i; D[tau_i] = 0; tau_i++;
            while (j != i) {
                tau[tau_i] = j; D[tau_i] = 0; tau_i++;
                if (k % t == 0) {
                    b[j] = 1; 
                }
                v[j] = 1; 
                j = pi[j]; k++;                
            }
            D[tau_i - 1] = 1;
            if (k > t) {
                b[i] = 1;
            }            
        }
    }

    // we add rank support to the bit vector
    rank_support_v<1> b_rank(&b);
    int rank = b_rank(n);
    //cout << rank << endl;
    vector<uint16_t> shortcuts(rank);

    // repeat the process of finding the cycles
    for (int i = 0; i <= n; i++) {
        if (v[i] == 1) { 
            v[i] = 0; int j = pi[i]; 
            while (v[j] == 1) {
                if (b[j] == 1) { 
                    shortcuts[b_rank(j + 1) - 1] = i; 
                    i = j; 
                }                   
                v[j] = 0; j = pi[j];                
            }
            if (b[j] == 1) {
                shortcuts[b_rank(j + 1) - 1] = i;
            }
            i = j;
        }
    }

    return {pi, shortcuts, b, b_rank, t, tau, D};
}


int inverse(permutation p, int i) {
    vector<uint16_t> pi = p.pi;
    vector<uint16_t> S = p.S;
    bit_vector b = p.b;
    rank_support_v<1> rank = p.rank;

    int j = i;
    bool s = true;
    while (pi[j] != i) {
        if (s && b[j] == 1) {
            s = false;
            j = S[rank(j)];
        } else {
            j = pi[j];
        }
    }
    return j;    
}


*/


    /*
    // Test case 1, book example
    permutation p1 = construct(pi1, 3);
    CUSTOM_ASSERT(p1.pi == pi1);
    CUSTOM_ASSERT(p1.S == vector<uint16_t>({7,8,1}));
    CUSTOM_ASSERT(p1.b == bit_vector({0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0}));
    CUSTOM_ASSERT(p1.tau == vector<uint16_t>({0, 9, 5,  1, 6, 10, 8, 3, 4, 7, 11,  2}));
    CUSTOM_ASSERT(p1.D == vector<uint16_t>({0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1}));
    CUSTOM_ASSERT(inverse(p1, 0) == 5);
    CUSTOM_ASSERT(inverse(p1, 1) == 11);
    CUSTOM_ASSERT(inverse(p1, 2) == 2);
    CUSTOM_ASSERT(inverse(p1, 3) == 8);

    // Test case 2, simple 1-forward permutation
    
    permutation p2 = construct(pi2, 2);
    CUSTOM_ASSERT(p2.pi == pi2);
    CUSTOM_ASSERT(p2.b == bit_vector({1, 0, 1, 0, 1}));
    CUSTOM_ASSERT(p2.S == vector<uint16_t>({4, 0, 2}));   
    CUSTOM_ASSERT(inverse(p2, 0) == 4);
    CUSTOM_ASSERT(inverse(p2, 1) == 0);

    // Test case 3, invert permutation
    
    permutation p3 = construct(pi3, 3);
    CUSTOM_ASSERT(p3.pi == pi3);
    CUSTOM_ASSERT(p3.S == vector<uint16_t>({}));
    CUSTOM_ASSERT(p3.b == bit_vector({0,0,0,0,0}));
    CUSTOM_ASSERT(inverse(p3, 0) == 4);
    CUSTOM_ASSERT(inverse(p3, 1) == 3);
    CUSTOM_ASSERT(inverse(p3, 2) == 2);*/