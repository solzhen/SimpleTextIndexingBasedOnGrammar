#include <sdsl/bit_vectors.hpp>

#include <iostream>

using namespace sdsl;
using namespace std;

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

    /*
    for (int i = 0; i < n; i++) {
        if (v[i] == 0) { // we found a new cycle
            int k = 0;
            int start_value = i;
            int current_value = i;
            while (true) {
                k++;            
                v[current_value] = 1;                
                current_value = pi[current_value]; 
                if (current_value == start_value) {
                    if (k > t)
                        b[current_value] = 1;
                    break;
                }          
                if (k % t == 0) {
                    b[current_value] = 1;
                }
            }
        }
    }*/

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
    // set visited elements to 0
    for (int i = 0; i < n; i++) {
        v[i] = 0;
    }
    // repeat the process of finding the cycles
    for (int i = 0; i < n; i++) {
        if (v[i] == 0) {
            int start_value = i;
            int current_value = i;
            int j = i;
            while (true) {
                v[current_value] = 1;
                current_value = pi[current_value];
                if (current_value == start_value) {
                    if (b[current_value] == 1) {
                        shortcuts[b_rank(current_value + 1) - 1] = j;
                    }
                    break;
                }
                if (b[current_value] == 1) {
                    shortcuts[b_rank(current_value + 1) - 1] = j;
                    j = current_value;
                }
            }
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

#include <cassert>

void test_main() {
    // Test case 1
    vector<uint16_t> pi1 = {9, 6, 2, 4, 7, 0, 10, 11, 3, 5, 8, 1};
    permutation p1 = construct(pi1, 3);
    assert(p1.pi == pi1);
    assert(p1.S == vector<uint16_t>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    assert(p1.b == bit_vector({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    assert(p1.t == 3);
    assert(inverse(p1, 0) == 5);
    assert(inverse(p1, 1) == 11);
    assert(inverse(p1, 2) == 6);
    assert(inverse(p1, 3) == 8);

    // Test case 2
    vector<uint16_t> pi2 = {1, 2, 3, 4, 5};
    permutation p2 = construct(pi2, 2);
    assert(p2.pi == pi2);
    assert(p2.S == vector<uint16_t>({0, 0, 0, 0, 0}));
    assert(p2.b == bit_vector({0, 0, 0, 0, 0}));
    assert(p2.t == 2);
    assert(inverse(p2, 0) == 1);
    assert(inverse(p2, 1) == 0);

    // Test case 3
    vector<uint16_t> pi3 = {5, 4, 3, 2, 1};
    permutation p3 = construct(pi3, 1);
    assert(p3.pi == pi3);
    assert(p3.S == vector<uint16_t>({0, 0, 0, 0, 0}));
    assert(p3.b == bit_vector({0, 0, 0, 0, 0}));
    assert(p3.t == 1);
    assert(inverse(p3, 0) == 4);
    assert(inverse(p3, 1) == 3);
    assert(inverse(p3, 2) == 2);

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