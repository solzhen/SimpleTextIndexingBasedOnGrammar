#include <sdsl/bit_vectors.hpp>

#include "permutations.hpp"
#include "sequences.hpp"

#include <iostream>

using namespace sdsl;
using namespace std;

#define ASSERT_EQUAL(expr1, expr2) \
    if ((expr1) != (expr2)) { \
        std::cerr << "Assertion failed: (" << #expr1 << " == " << #expr2 << ") evaluated as (" << (expr1) << " != " << (expr2) << ")\n\t" << __FILE__ << ", line " << __LINE__ << std::endl; \
        std::abort(); \
    } else { \
        std::cout << "Success: " << #expr1 << " : " << (expr1) << "  == " << (expr2) << ",\n\t" << __FILE__ << ", line " << __LINE__ << std::endl; \
    }

Sequence::Sequence(int_vector<> S, int sigma) : sigma(sigma) {
    int_vector<> F(sigma + 1, 0); // array F[0, sigma] of integers/symbols in the alphabet
    n = S.size(); // length of the sequence
    for (int i = 0; i < n; i++) {
        F[S[i]]++; // count the number of occurrences of each symbol
    }
    cout << "F = ";
    for (int i = 0; i < sigma; i++) {
        cout << F[i] << " ";
    } cout << endl;
    int noc = ceil(static_cast<double>(n) / sigma); // number of chunks to divide the sequence into
    cout << "sigma = " << sigma << ", n = " << n << ", noc = " << noc << endl;
    cout << "chunks = ";
    for (int i = 0; i < noc; i++) {
        for (int j = i * sigma; j < min((i+1) * sigma, n); j++) {
            cout << S[j] << " ";
        } cout << ", ";
    } cout << endl;
    A = vector<abv>(sigma);
    for (int i = 0; i < sigma; i++) {
        A[i].b = bit_vector(F[i] + noc - 1, 0);
        F[i] = 0; // reset f
    }
    for (int k = 0; k < noc; k++) {
        for (int i = k * sigma; i < min((k+1) * sigma, n); i++) {
            A[S[i]].b[F[S[i]]] = 1; // bitset
            F[S[i]]++;                
        }
        for (int c = 0; c < sigma; c++) {
            A[c].b[F[c]] = 0; // bitclear
            F[c]++;                
        }
    }
    cout << "A = ";
    for (int i = 0; i < sigma; i++) {
        cout << A[i].b << " ";
    } cout << endl;

    vector<int_vector<>> pi_v(noc);
    D = vector<dbv>(noc);
    for (int k = 0; k < noc; k++) { // for each chunk
        for (int c = 0; c <= sigma; c++) {
            F[c] = 0; // reset f
        }
        int l = min((k+1) * sigma, n) - k * sigma; // length of the chunk

        pi_v[k] = int_vector<>(l, 0, ceil(log2(sigma)));
        D[k].b = bit_vector(l + sigma, 0);
        for (int i = k * sigma; i < min((k+1) * sigma, n); i++) {
            F[S[i]]++; // count the number of occurrences of each symbol in chunk
        }            
        for (int c = 0; c < sigma; c++) {
            if (c > 0) // c-1 is out of bounds
                F[c] = F[c] + F[c - 1]; // accumulate the number of occurrences
            D[k].b[F[c] + c] = 1; // mark with 1
        }
        //cout << "D[" << k << "] = " << D[k] << endl;
        for (int i = k * sigma; i < min((k+1) * sigma, n); i++) {
            int prev_symbol = (S[i] == 0) ? sigma : S[i]-1; // the pain of doing 0-based indexing
            pi_v[k][F[prev_symbol]] = i - k * sigma; // position in the chunk
            F[prev_symbol]++; // offset in pi_k
            //D[k][F[prev_symbol] + S[i] - 1] = 0; 
        }
    }
    // pi: 4, 0 2, 1 3, 5, -, 6, - ;; 6, 1 4, 5, 2, 3, -, 0 ;; 4, -, 1, -, 2 3, -, 0   
    // D: 01001001011011 01001010101101 011011001101  
    cout << "D = ";
    for (int i = 0; i < noc; i++) {
        cout << D[i].b << " ";
    } cout << endl;
    cout << "pi = ";
    for (int i = 0; i < noc; i++) {
        cout << pi_v[i] << " , ";
    } cout << endl;

    for (int i = 0; i < sigma; i++) {
        A[i].rank = rank_support_v<0>(&A[i].b);
        A[i].sel_1 = select_support_mcl<1, 1>(&A[i].b);
        A[i].sel_0 = select_support_mcl<0, 1>(&A[i].b);
    }
    for (int i = 0; i < noc; i++) {
        D[i].sel_1 = select_support_mcl<1, 1>(&D[i].b);
        D[i].sel_0 = select_support_mcl<0, 1>(&D[i].b);
    }
    pi = vector<Permutation>(noc);
    for (int i = 0; i < noc; i++) {
        pi[i] = Permutation(pi_v[i], 3);
    }
};
int Sequence::select_1_D(int k, int i) {
    if (i == 0) return -1;
    return D[k].sel_1(i);
}
int Sequence::select_0_D(int k, int i) {
    if (i == 0) return -1;
    return D[k].sel_0.select(i);
}
int Sequence::select_0_A(int k, int i) {
    if (i == 0) return -1;
    return A[k].sel_0.select(i);
}
int Sequence::select_1_A(int k, int i) {
    if (i == 0) return -1;
    return A[k].sel_1(i);
}
int Sequence::access(int i) {
    if (i < 0 || i >= n) return -1;
    int k = i / sigma;
    int i_prime = (i % sigma);
    int j = pi[k].inverse(i_prime);
    int s = select_0_D(k, j+1);
    return s - j;
}
int Sequence::rank(int c, int i) {
    int k = i / sigma;
    int i_prime = (i % sigma);
    int sL = select_1_D(k, c) - (c);
    int eL = select_1_D(k, c+1) - c - 1;
    // binary search [sL+1, eL] for the largest j such that
    // read (pi_k, j) <= i' (set j = sL if read (pi_k, sL) > i')
    int j = sL;
    int left = sL+1; int right = eL;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int read = pi[k][mid];
        if (read == i_prime) {
            j = mid;
            break;
        } else if (read < i_prime) {
            j = mid;
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return select_0_A(c, k) - (k) + 1 + (j - sL);
}
int Sequence::rank_A(int c, int i) {
    return A[c].rank(i);
}
int Sequence::pred_0_A(int c, int s) {
    if (s == 0) return -1;
    int r = rank_A(c, s+1); // rank_0
    return select_0_A(c, r);
}
int Sequence::select(int c, int j) {
    if (j == 0) return -1;
    int s = select_1_A(c, j);
    if (s > A[c].b.size()) return n;
    int pred = pred_0_A(c, s);
    int j_prime = s - pred;
    int sL = select_1_D(s+1-j, c ) - (c); // s-j :: which chunk
    return (s+1-j)*sigma + pi[s+1-j].permute(j_prime + sL);
};


void test() {       //0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18
    int_vector<> S = {1, 2, 1, 2, 0, 3, 5, 6, 1, 3, 4, 1, 2, 0, 6, 2, 4, 4, 0};
    cout << "S = " << S << endl;
    cout << "Creating sequence structure..." << endl;
    Sequence seq(S, 7);
    ASSERT_EQUAL(seq.rank(0, 0), 0);
    ASSERT_EQUAL(seq.rank(0, 3), 0);
    ASSERT_EQUAL(seq.rank(0, 4), 1);
    ASSERT_EQUAL(seq.rank(0, 10), 1);
    ASSERT_EQUAL(seq.rank(0, 13), 2);
    ASSERT_EQUAL(seq.rank(0, 14), 2);
    ASSERT_EQUAL(seq.rank(0, 18), 3);
    ASSERT_EQUAL(seq.rank(1, 0), 1);
    ASSERT_EQUAL(seq.rank(1, 2), 2);
    ASSERT_EQUAL(seq.rank(1, 7), 2);
    ASSERT_EQUAL(seq.rank(1, 8), 3);
    ASSERT_EQUAL(seq.rank(1, 11), 4);
    ASSERT_EQUAL(seq.rank(1, 18), 4);
    ASSERT_EQUAL(seq.rank(2, 3), 2);
    ASSERT_EQUAL(seq.rank(2, 15), 4);
    ASSERT_EQUAL(seq.rank(3, 10), 2);
    ASSERT_EQUAL(seq.rank(4, 16), 2);
    ASSERT_EQUAL(seq.rank(5, 6), 1);
    ASSERT_EQUAL(seq.rank(6, 7), 1);
    ASSERT_EQUAL(seq.rank(6, 14), 2);

    cout << "All tests passed!" << endl;

}
/*
g++ -std=c++11 -O3 -DNDEBUG -I . -I ~/include -c permutations.cpp -o permutations.o;
g++ -std=c++11 -O3 -DNDEBUG -I . -I ~/include -c sequences.cpp -o sequences.o;
g++ -std=c++11 -O3 -DNDEBUG -L ~/lib sequences.o permutations.o -o sequences.x -lsdsl -ldivsufsort -ldivsufsort64;
./sequences.x
*/