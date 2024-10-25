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

bool isPermutation(int_vector<> pi) {
    int n = pi.size();
    bit_vector v(n, 0);
    for (int i = 0; i < n; i++) {
        if (pi[i] < 0 || pi[i] >= n) {
            return false;
        }
        if (v[pi[i]] == 1) {
            return false;
        }
        v[pi[i]] = 1;
    }
    for (int i = 0; i < n; i++) {
        if (v[i] == 0) {
            return false;
        }
    }
    return true;
}

Permutation::Permutation() : t(0) {}
Permutation::Permutation(int_vector<> pi, int t) : pi(pi), t(t) {    
    if (!isPermutation(pi)) {
        throw invalid_argument("The input vector is not a permutation");
    }    
    int n = pi.size();
    bit_vector v(n, 0);
    bit_vector bb(n, 0);
    for (int i = 0; i < n; i++) {
        if (v[i] == 0) {
            v[i] = 1; 
            int j = pi[i]; int k = 1;
            while (j != i) {
                if (k % t == 0) {
                    bb[j] = 1; 
                }
                v[j] = 1; 
                j = pi[j]; k++;
            }
            if (k > t) {
                bb[i] = 1;
            }            
        }
    }
    this->b = brv();
    this->b.b = bb;
    this->b.rank = rank_support_v<1>(&b.b);
    int rank = b.rank(n);
    int_vector<> shortcuts(rank);
    for (int i = 0; i <= n; i++) {
        if (v[i] == 1) { 
            v[i] = 0; int j = pi[i]; 
            while (v[j] == 1) {
                if (bb[j] == 1) { 
                    shortcuts[b.rank(j + 1) - 1] = i; 
                    i = j; 
                }                   
                v[j] = 0; j = pi[j];                
            }
            if (bb[j] == 1) {
                shortcuts[b.rank(j + 1) - 1] = i;
            }
            i = j;
        }
    }
    this->S = shortcuts;
};
int Permutation::operator[](int i) {
    return pi[i];
}
int Permutation::inverse(int i) {
    // check if b.rank is null
    if (b.rank.size() != b.b.size()) {
        cout << "erm" << endl;
    }
    if (i >= this->pi.size()) {
        return -1;
    }
    int j = i; bool s = true;

    int loop = 0;
    
    while (pi[j] != i) {
        loop++;
        //cout << "j: " << j << endl;
        if (s && b.b[j] == 1) {
            s = false;
            //int aux = b.rank(j+1);
            //cout << "aux: " << aux << endl;
            j = S[b.rank(j+1) - 1];
            //cout << "j: " << j << endl;
        } else {
            j = pi[j];
        }
        if (loop > 20) {
            break;
        }
    } //cout << endl;
    return j;
}
int Permutation::rank_b(int i) {
    return b.rank(i+1);
}
PowerPermutation::PowerPermutation(int_vector<> pi, int t) : Permutation(pi, t) {
    int n = pi.size();
    bit_vector v(n, 0);
    int_vector<> tau_v(n); 
    bit_vector  DD(n); 
    int tau_i = 0;
    for (int i = 0; i < n; i++) {
        if (v[i] == 0) {
            v[i] = 1; 
            int j = pi[i]; int k = 1;
            tau_v[tau_i] = i; DD[tau_i] = 0; tau_i++;
            while (j != i) {
                tau_v[tau_i] = j; DD[tau_i] = 0; tau_i++;
                v[j] = 1; 
                j = pi[j]; k++;                
            }
            DD[tau_i - 1] = 1;
        }
    }
    this->tau = Permutation(tau_v, t);
    this->D.b = DD;
    this->D.rank = bit_vector::rank_1_type(&D.b);
    this->D.sel = bit_vector::select_1_type(&D.b);
}
int PowerPermutation::power(int i, int k) {
    if (!vectorset) {
        tau.b.rank.set_vector(&tau.b.b);
        vectorset = true;
    }
    int j = tau.inverse(i);
    int chunk_number = this->rank_D(j - 1); //[0..j-1]
    int pred = this->select_D(chunk_number) + 1; // [0..]
    int succ = this->select_D(chunk_number + 1);
    int ind = pred + ( (j-pred + k) % (succ - pred + 1) );
    return tau[ ind ];
}
int PowerPermutation::rank_D(int i) {
    return this->D.rank.rank(i+1); //sdsl rank return [0::i) exclusive i
}
int PowerPermutation::select_D(int i) {
    if (i==0) return -1;
    return this->D.sel.select(i);
}

void test_main() {    
    int_vector<> pi2 = {1, 2, 3, 4, 0};    
    Permutation pi2i = Permutation(pi2, 2);
    ASSERT_EQUAL(pi2i.inverse(0), 4);
    ASSERT_EQUAL(pi2i.inverse(1), 0);
    int_vector<> pi3 = {4, 3, 2, 1, 0};
    Permutation pi3i = Permutation(pi3, 3);
    ASSERT_EQUAL(pi3i.inverse(0), 4);
    ASSERT_EQUAL(pi3i.inverse(1), 3);
    ASSERT_EQUAL(pi3i.inverse(2), 2);
    int_vector<> ppp = {9, 6, 2, 4, 7, 0, 10, 11, 3, 5, 8, 1};
    Permutation pppi = Permutation(ppp, 3);
    PowerPermutation power_perm = PowerPermutation(ppp, 3);
    //ASSERT_EQUAL(power_perm.tau.pi, int_vector<>({0,9,5,1,6,10,8,3,4,7,11,2}));
    //ASSERT_EQUAL(power_perm.D, bit_vector({0,0,1,0,0,0,0,0,0,0,1,1}));
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
    int_vector<> pi; // permutation
    int_vector<> S; // shortcuts
    bit_vector b; // bit vector to mark shortcuts
    rank_support_v<1> rank; // rank support for b
    int t; // parameter t
    int_vector<> tau; // permutation induced by the cycle decomposition
    int_vector<> D; // bitvector D
};

struct permutation construct(int_vector<> pi, int t) {
    int n = pi.size();
    // bit vector to mark visited elements
    bit_vector v(n, 0); 
    // bit vector to mark the shortcuts, where B[i]==1 iff there is a shortcut
    // leaving from i
    bit_vector b(n, 0); 


    // let C1, C2, ... Cc be the cycle decomposition of pi, and l_i = |Ci|
    // let tau = C1 C2 ... Cc be the concatenation of the cycles 
    // and a bitvector D = 0^(l_1-1) 1 0^(l_2-1) 1 ... 0^(l_c-1) 1
    int_vector<> tau(n); // permutation induced by the cycle decomposition
    int_vector<> D(n); // bitvector D

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
    int_vector<> shortcuts(rank);

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
    int_vector<> pi = p.pi;
    int_vector<> S = p.S;
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
    CUSTOM_ASSERT(p1.S == int_vector<>({7,8,1}));
    CUSTOM_ASSERT(p1.b == bit_vector({0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0}));
    CUSTOM_ASSERT(p1.tau == int_vector<>({0, 9, 5,  1, 6, 10, 8, 3, 4, 7, 11,  2}));
    CUSTOM_ASSERT(p1.D == int_vector<>({0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1}));
    CUSTOM_ASSERT(inverse(p1, 0) == 5);
    CUSTOM_ASSERT(inverse(p1, 1) == 11);
    CUSTOM_ASSERT(inverse(p1, 2) == 2);
    CUSTOM_ASSERT(inverse(p1, 3) == 8);

    // Test case 2, simple 1-forward permutation
    
    permutation p2 = construct(pi2, 2);
    CUSTOM_ASSERT(p2.pi == pi2);
    CUSTOM_ASSERT(p2.b == bit_vector({1, 0, 1, 0, 1}));
    CUSTOM_ASSERT(p2.S == int_vector<>({4, 0, 2}));   
    CUSTOM_ASSERT(inverse(p2, 0) == 4);
    CUSTOM_ASSERT(inverse(p2, 1) == 0);

    // Test case 3, invert permutation
    
    permutation p3 = construct(pi3, 3);
    CUSTOM_ASSERT(p3.pi == pi3);
    CUSTOM_ASSERT(p3.S == int_vector<>({}));
    CUSTOM_ASSERT(p3.b == bit_vector({0,0,0,0,0}));
    CUSTOM_ASSERT(inverse(p3, 0) == 4);
    CUSTOM_ASSERT(inverse(p3, 1) == 3);
    CUSTOM_ASSERT(inverse(p3, 2) == 2);*/