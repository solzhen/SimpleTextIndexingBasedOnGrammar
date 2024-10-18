#include <sdsl/bit_vectors.hpp>

#include <iostream>

using namespace sdsl;
using namespace std;

#include <iostream>
#include <cstdlib>

// Custom assert macro
#define CUSTOM_ASSERT(expr) \
    if (!(expr)) { \
        std::cerr << "Assertion failed: " << #expr << ", file " << __FILE__ << ", line " << __LINE__ << std::endl; \
        std::abort(); \
    }

struct permutation {
    vector<uint16_t> pi; // permutation
    vector<uint16_t> S; // shortcuts
    bit_vector b; // bit vector to mark shortcuts
    rank_support_v<1> rank; // rank support for b
    int t; // parameter t
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
    // and a bitvector D = 0^l_1 1 0^l_2 1 ... 0^l_c 1
    vector<uint16_t> tau(n); // permutation induced by the cycle decomposition
    vector<uint16_t> D(n); // bitvector D


    for (int i = 0; i < n; i++) {
        if (v[i] == 0) {
            v[i] = 1; // bitset v[i]
            int j = pi[i]; int k = 1;
            while (j != i) {
                if (k % t == 0) {
                    b[j] = 1; // bitset b[j]
                }
                v[j] = 1; // bitset v[j]
                j = pi[j]; k++;
            }
            if (k > t) {
                b[i] = 1; // bitset b[i]
            }            
        }
    }


    // we add rank support to the bit vector
    rank_support_v<1> b_rank(&b);
    int rank = b_rank(n);
    //cout << rank << endl;
    vector<uint16_t> shortcuts(rank);
    // repeat the process of finding the cycles
    
    for (int i = 1; i <= n; i++) {
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

    return {pi, shortcuts, b, b_rank, t};
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

void test_main() {
    // Test case 1, book example
    vector<uint16_t> pi1 = {9, 6, 2, 4, 7, 0, 10, 11, 3, 5, 8, 1};
    permutation p1 = construct(pi1, 3);
    CUSTOM_ASSERT(p1.pi == pi1);
    CUSTOM_ASSERT(p1.S == vector<uint16_t>({7,8,1}));
    CUSTOM_ASSERT(p1.b == bit_vector({0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0}));
    CUSTOM_ASSERT(inverse(p1, 0) == 5);
    CUSTOM_ASSERT(inverse(p1, 1) == 11);
    CUSTOM_ASSERT(inverse(p1, 2) == 2);
    CUSTOM_ASSERT(inverse(p1, 3) == 8);

    // Test case 2, simple 1-forward permutation
    vector<uint16_t> pi2 = {1, 2, 3, 4, 0};
    permutation p2 = construct(pi2, 2);
    CUSTOM_ASSERT(p2.pi == pi2);
    CUSTOM_ASSERT(p2.b == bit_vector({1, 0, 1, 0, 1}));
    CUSTOM_ASSERT(p2.S == vector<uint16_t>({4, 1, 2}));   
    CUSTOM_ASSERT(inverse(p2, 0) == 4);
    CUSTOM_ASSERT(inverse(p2, 1) == 0);

    // Test case 3, invert permutation
    vector<uint16_t> pi3 = {4, 3, 2, 1, 0};
    permutation p3 = construct(pi3, 3);
    CUSTOM_ASSERT(p3.pi == pi3);
    CUSTOM_ASSERT(p3.S == vector<uint16_t>({}));
    CUSTOM_ASSERT(p3.b == bit_vector({0,0,0,0,0}));
    CUSTOM_ASSERT(inverse(p3, 0) == 4);
    CUSTOM_ASSERT(inverse(p3, 1) == 3);
    CUSTOM_ASSERT(inverse(p3, 2) == 2);

    cout << "All tests passed!" << endl;
}

int main() {
    /*
    vector<uint16_t> pi = {9,6,2,4,7,0,10,11,3,5,8,1};
    permutation p = construct(pi, 3);
    // print p
    for (int i = 0; i < p.pi.size(); i++) {
        cout << p.pi[i] << " ";
    } cout << endl;
    // print S
    for (int i = 0; i < p.S.size(); i++) {
        cout << p.S[i] << " ";
    } cout << endl;
    // print b
    for (int i = 0; i < p.b.size(); i++) {
        cout << p.b[i] << " ";
    } cout << endl;*/

    test_main();
    return 0;
}




// g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib permutations.cpp -o permutations.o -lsdsl -ldivsufsort -ldivsufsort64