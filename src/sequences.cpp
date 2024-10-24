#include <sdsl/bit_vectors.hpp>

#include "permutations.hpp"

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

typedef struct abv {
    bit_vector b;
    rank_support_v<0> rank;
    select_support_mcl<1, 1> sel_1;
    select_support_mcl<0, 1> sel_0;
} abv;

typedef struct dbv {
    bit_vector b;
    select_support_mcl<1, 1> sel_1;
    select_support_mcl<0, 1> sel_0;
} dbv;

class Sequence {
private:
public:
    vector<abv> A;
    vector<dbv> D;
    //vector<bit_vector> A;
    vector<Permutation> pi;
    //vector<bit_vector> D;
    int sigma; int n;
public:
    /// @brief Builds permutation representation of S
    /// @param S The sequence of characters as a vector of integers
    /// @param sigma Length of alphabet [0 .. sigma)
    Sequence(int_vector<> S, int sigma) : sigma(sigma) {
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
    int select_1_D(int k, int i) {
        if (i == 0) return -1;
        return D[k].sel_1(i);
    }
    int select_0_D(int k, int i) {
        if (i == 0) return -1;
        return D[k].sel_0.select(i);
    }
    int select_0_A(int k, int i) {
        if (i == 0) return -1;
        return A[k].sel_0.select(i);
    }
    int select_1_A(int k, int i) {
        if (i == 0) return -1;
        return A[k].sel_1(i);
    }
    int access(int i) {
        if (i < 0 || i >= n) return -1;
        int k = i / sigma;
        int i_prime = (i % sigma);
        int j = pi[k].inverse(i_prime);
        int s = select_0_D(k, j+1);
        return s - j;
    }
    int rank(int c, int i) {
        int k = i / sigma;
        int i_prime = (i % sigma);
        int sL = select_1_D(k, c - 1) - (c - 1);
        int sLL = sL;
        int eL = select_1_D(k, c) - c;
        // binary search [sL+1, eL] for the largest j such that
        // read (pi_k, j) <= i' (set j = sL if read (pi_k, sL) > i')
        int j = sL;
        while (sL < eL) {
            int m = (sL + eL) / 2;
            if (pi[k][m] <= i_prime) {
                j = m;
                sL = m + 1;
            } else {
                eL = m;
            }
        }
        return select_0_A(c, k) - (k) + (j - sLL);
    }
    int rank_A(int c, int i) {
        return A[c].rank(i);
    }
    int pred_0_A(int c, int s) {
        int r = rank_A(c, s+1);
        return select_0_A(c, r) + 1;
    }
    int select(int c, int j) {
        if (j == 0) return -1;
        int s = select_1_A(c, j);
        if (s > A[c].b.size()) return n;
        int j_prime = s - pred_0_A(c, s);
        int sL = select_1_D(s-j, c-1) - (c-1); // s-j :: which chunk
        return (s-j)*sigma + pi[s-j].permute(j_prime + sL);
    }
};

void test() {
    int_vector<> S = {1, 2, 1, 2, 0, 3, 5, 6, 1, 3, 4, 1, 2, 0, 6, 2, 4, 4, 0};
    cout << "S = " << S << endl;
    cout << "Creating sequence structure..." << endl;
    Sequence seq(S, 7);
    ASSERT_EQUAL(seq.access(0), 1);
    ASSERT_EQUAL(seq.access(1), 2);
    ASSERT_EQUAL(seq.access(2), 1);
    ASSERT_EQUAL(seq.access(3), 2);
    ASSERT_EQUAL(seq.access(4), 0);
    ASSERT_EQUAL(seq.access(5), 3);
    ASSERT_EQUAL(seq.access(6), 5);
    ASSERT_EQUAL(seq.access(7), 6);
    ASSERT_EQUAL(seq.access(8), 1);
    ASSERT_EQUAL(seq.access(9), 3);
    ASSERT_EQUAL(seq.access(10), 4);
    ASSERT_EQUAL(seq.access(11), 1);
    ASSERT_EQUAL(seq.access(12), 2);
    ASSERT_EQUAL(seq.access(13), 0);
    ASSERT_EQUAL(seq.access(14), 6);
    ASSERT_EQUAL(seq.access(15), 2);
    ASSERT_EQUAL(seq.access(16), 4);
    ASSERT_EQUAL(seq.access(17), 4);
    ASSERT_EQUAL(seq.access(18), 0);
    ASSERT_EQUAL(seq.access(19), -1);

    cout << "All tests passed!" << endl;

}

int main() {
    test();
}

/*
g++ -std=c++11 -O3 -DNDEBUG -I . -I ~/include -c permutations.cpp -o permutations.o;
g++ -std=c++11 -O3 -DNDEBUG -I . -I ~/include -c sequences.cpp -o sequences.o;
g++ -std=c++11 -O3 -DNDEBUG -L ~/lib sequences.o permutations.o -o sequences.x -lsdsl -ldivsufsort -ldivsufsort64;
./sequences.x
*/