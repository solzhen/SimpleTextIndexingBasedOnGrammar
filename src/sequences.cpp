#include <sdsl/bit_vectors.hpp>

#include <iostream>

using namespace sdsl;
using namespace std;

int main() {
    int n = 20; //length of the sequence
    int sigma = 20; //alphabet size
    vector<uint16_t> seq(n);
    //fill seq with random values between 0 and sigma
    for (int i = 0; i < n; i++) {
        seq[i] = rand() % sigma;
    }
    //allocate array f[0, sigma] of integers, 
    //to count the number of occurrences of each symbol
    vector<int> f(sigma + 1, 0);
    for (int i = 0; i < n; i++) {
        f[seq[i]]++; //count the number of occurrences of each symbol
    }
    int noc = ceil(n / sigma); //number of chunks to divide the sequence into
    // initializes sigma bit_arrays, 1 for each symbol
    // each array is of size f[c] + ceil(n / sigma) -1
    // they will record, in unary, the number of occurrences of each symbol
    // in each chunk
    vector<bit_vector> a(sigma);
    for (int i = 0; i < sigma; i++) {
        a[i] = bit_vector(f[i] + noc - 1, 0);
        f[i] = 0; //reset f
    }
    //for each chunk
    for (int k = 0; k < noc; k++) {
        //for each symbol in the chunk
        for (int i = k * sigma; i <= min((k + 1) * sigma, n); i++) {
            f[seq[i]]++;
            a[seq[i]][f[seq[i - 1]]] = 1; // bitset
        }
        // for each symbol in the alphabet
        for (int c = 0; c < sigma; c++) {
            f[c]++;
            a[c][f[c]] = 0; // bitclear
        }
    }
    
    // a chunck C[0, sigma] will be represented as the permutation induced by
    // its inverted index
    // let L[c] be the sequence of the positions of symbol c in C, in increasing order,
    // then consider the permutation pi = L[0] L[1] ... L[sigma-1] on [0, sigma], there 
    // may be empty lists, so we'll use a bitvector D to mark the starting
    // positions of the lists in pi with this form:
    // D = 0 ^ |L[0]| 1 0 ^ |L[1]| 1 ... 0 ^ |L[sigma-1]| 1
    // we can see that
    // L[c] = pi[select_1(D, c-1) - (c-1) + 1, select_1(D, c) - c] 


    int huh = ceil(log2(sigma));

    vector<int_vector<>> pi(noc);
    vector<bit_vector> d;

    // for each chunk
    for (int k = 0; k < noc; k++) {
        for (int c = 0; c < sigma; c++) {
            f[c] = 0; //    reset f
        }
        int l = min((k+1) * sigma, n) - k * sigma; // length of the chunk
        pi[k] = int_vector<> (l, 0, huh);
        d[k] = bit_vector(l + sigma, 0); 


        for (int i = k * sigma; i < min((k + 1) * sigma, n); i++) {
            f[seq[i]]++;
        }
        for (int c = 0; c < sigma; c++) {
            f[c] = f[c] + f[c - 1];
            d[k][f[c] + c + 1] = 1;
        }
        // for each symbol in the chunk
        for (int i = k * sigma; i < min((k + 1) * sigma, n); i++) {
            f[seq[i] - 1]++ ; 
            pi[k][f[seq[i] - 1]] = i - (k-1)*sigma;
            d[k][f[seq[i] - 1] + seq[i] - 1] = 0;
        }
    }
    // free f
    f.clear();
    // preprocess all a for select_0, select_1 and pred_0 queries
    vector<select_support_mcl<1, 1>> sel_1_a(sigma);
    for (int i = 0; i < sigma; i++) {
        sel_1_a[i] = select_support_mcl<1, 1>(&a[i]);
    }
    vector<select_support_mcl<0, 1>> sel_0_a(sigma);
    for (int i = 0; i < sigma; i++) {
        sel_0_a[i] = select_support_mcl<0, 1>(&a[i]);
    }

    
    // preprocess al d for select_0, select_1 queries
    vector<select_support_mcl<1, 1>> sel_1_d(noc);
    for (int i = 0; i < noc; i++) {
        sel_1_d[i] = select_support_mcl<1, 1>(&d[i]);
    }
    vector<select_support_mcl<0, 1>> sel_0_d(noc);
    for (int i = 0; i < noc; i++) {
        sel_0_d[i] = select_support_mcl<0, 1>(&d[i]);
    }
    

}

/*



*/