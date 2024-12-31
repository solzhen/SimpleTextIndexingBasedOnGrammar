#include "nsequences.hpp"

using namespace sdsl;
using namespace std;

RSequence::RSequence()
{
}

RSequence::RSequence(int_vector<> S, int sigma) : sigma(sigma){
    int_vector<> F(sigma + 1, 0); 
    n = S.size();
    for (int i = 0; i < n; i++) {
        F[S[i]]++;
    }
    int noc = ceil(static_cast<double>(n) / sigma);

    A_ = abv();
    A_.b = bit_vector(2*n, 0);
    B_ = rbv(); // marks the start of each chunk in A_ with a bit vector
    B_.b = bit_vector(2*n+1, 0);

    int_vector<0> C_ = int_vector<0>(sigma+1, 0); // position in A where each chunk starts  
    // now each i-th chunk starts in A_[C_[i]] and ends in A_[C_[i+1]-1]
    for (int i = 0; i < sigma; i++) {
        C_[i+1] = C_[i] + F[i] + noc - 1;
        B_.b[C_[i]] = 1;    
        F[i] = 0; // reset f
    } 
    B_.b[C_[sigma]] = 1;
    B_.rank = rank_support_v<1, 1>(&B_.b);
    B_.sel = select_support_mcl<1, 1>(&B_.b); 

    // now we use F to hold the index for each chunk in the loop below
    for (int k = 0; k < noc; k++) {
        for (int i = k * sigma; i < min((k+1) * sigma, n); i++) {
            A_.b[C_[S[i]] + F[S[i]]] = 1; // bit set
            F[S[i]]++;
        }
        for (int c = 0; c < sigma; c++) {
            if (C_[c] + F[c] == C_[c+1]) continue; // we finished the chunk-
            A_.b[C_[c] + F[c]] = 0; // we add a 0 to the end of each chunk
            F[c]++;
        }
    }

    A_.rank = rank_support_v<0>(&A_.b);
    A_.sel_1 = select_support_mcl<1, 1>(&A_.b);
    A_.sel_0 = select_support_mcl<0, 1>(&A_.b);

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
    //cout << "D = "; for (int i = 0; i < noc; i++) cout << D[i].b << " "; cout << endl;
    //cout << "pi = "; for (int i = 0; i < noc; i++) cout << pi_v[i] << " "; cout << endl;
    for (int i = 0; i < noc; i++) {
        D[i].sel_1 = select_support_mcl<1, 1>(&D[i].b);
        D[i].sel_0 = select_support_mcl<0, 1>(&D[i].b);
    }
    pi = vector<Permutation>(noc);
    for (int i = 0; i < noc; i++) {
        pi[i] = Permutation(pi_v[i], 3);
    }
    // A_k = A_[ B_.sel_1(k+1) : B_.sel_1(k+2) - 1 ]
    // A_k.rank_1(i) = A_.rank_1(i +  B_.sel_1(k+1)) - A_.rank_1(B_.sel_1(k+1))
    // A_k.select_1(j) = A_.select_1(j + A_.rank_1(B_.sel_1(k+1))) - B_.sel_1(k+1)
    
}

int RSequence::select_1_D(int k, int i) {
    if (i == 0) return -1;
    return D[k].sel_1(i);
}
int RSequence::select_0_D(int k, int i) {
    if (i == 0) return -1;
    return D[k].sel_0.select(i);
}

int RSequence::access(int i) {
    if (i < 0 || i >= n) return -1;
    if (sigma == 0) {
        std::cerr << "Error: sigma is zero, division by zero." << std::endl;
        return -1;
    }
    int k = i / sigma;
    int i_prime = (i % sigma);
    assert(k >= 0 && k < pi.size() && "k is out of bounds");
    if (k < 0 || k >= pi.size()) {
        std::cerr << "Error: k is out of bounds. k = " << k << ", pi.size() = " << pi.size() << std::endl;
        return -1;
    }
    int j = pi[k].inverse(i_prime);
    assert(j >= 0 && "inverse function returned a negative value");
    if (j < 0) {
        std::cerr << "Error: inverse function for i=" << i_prime << " returned a negative value. j = " << j << std::endl;
        return -1;
    }
    int s = select_0_D(k, j + 1);
    assert(s >= 0 && "select_0_D function returned a negative value");
    if (s < 0) {
        std::cerr << "Error: select_0_D function returned a negative value. s = " << s << std::endl;
        return -1;
    }
    return s - j;
}

// A_k.select_1(j) = A_.select_1(j + A_.rank(B_.sel(k+1))) - B_.sel(k+1)
int RSequence::select_0_A(int k, int i){
    if (i == 0) return -1;
    int C_k = B_.sel(k+1);
    return A_.sel_0(i + A_.rank(C_k)) - C_k;
}

int RSequence::rank(int c, int i) {
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

//A_k.select_1(j) = A_.select_1(j + A_.rank_1(B_.sel_1(k+1))) - B_.sel_1(k+1)
int RSequence::select_1_A(int k, int i) {
    if (i == 0) return -1;
    int C_k = B_.sel(k+1);
    return A_.sel_1(i + C_k - A_.rank(C_k))  - C_k;
}
// A_k.rank_1(i) = A_.rank_1(i +  B_.sel_1(k+1)) - A_.rank_1(B_.sel_1(k+1))
int RSequence::rank_A(int c, int i) {
    int C_k = B_.sel(c+1);
    return A_.rank(i + C_k) - A_.rank(C_k);
}

int RSequence::pred_0_A(int c, int s) {
    if (s == 0) return -1;
    int r = rank_A(c, s+1); // rank_0
    return select_0_A(c, r);
}


//A_k.size() = B_.sel_1(k+2) - B_.sel_1(k+1)
int RSequence::select(int c, int j) {
    if (j == 0) return -1;
    int s = select_1_A(c, j);
    if (s >= (B_.sel(c+2) - B_.sel(c+1))) return n; // A_c.size()
    int pred = pred_0_A(c, s);
    int j_prime = s - pred;
    int sL = select_1_D(s+1-j, c ) - (c); // s-j :: which chunk
    return (s+1-j)*sigma + pi[s+1-j].permute(j_prime + sL);
}

/*
Algorithm 6.2: Building the permutation-based representation.
Input : Sequence S[1, n] over alphabet [1, σ ].
Output: Builds the permutation representation of S: Ac and Sk (as1 Allocate array F[0, σ ] of integers
2 for c ← 1 to σ do
 F[c] ← 0
3 for i ← 1 to n do
4
 F[S[i]] ← F[S[i]] + 1
5 for c ← 1 to σ do
6
 Allocate bit array Ac [1, F[c] + n/σ − 1]
7
 F[c] ← 0
8 for k ← 1 to
 n/σ do
9
 for i ← (k − 1)σ + 1 to min(kσ, n) do
10
 F[S[i]] ← F[S[i]] + 1
11
 bitset(AS[i] , F[S[i]])
12
 for c ← 1 to σ do
13
 F[c] ← F[c] + 1
14
 bitclear(Ac , F[c])
15 for k ← 1 to
 n/σ do
16
 for c ← 0 to σ do F[c] ← 0
17
 l ← min(kσ, n) − (k − 1)σ
18
 Allocate array πk [1, l] of log σ -bit values
19
 Allocate bit array Dk [1, l + σ ]
20
 for i ← (k − 1)σ + 1 to min(kσ, n) do
21
 F[S[i]] ← F[S[i]] + 1
22
 for c ← 1 to σ do
23
 F[c] ← F[c] + F[c − 1]
24
 bitset(Dk , F[c] + c)
25
 for i ← (k − 1)σ + 1 to min(kσ, n) do
26
 F[S[i] − 1] ← F[S[i] − 1] + 1
27
 write(πk , F[S[i] − 1], i − (k − 1)σ )
28
 bitclear(Dk , F[S[i] − 1] + S[i] − 1)
29 Free F and (if desired) S
30 Preprocess all Ac for select0/1 and pred0 queries
31 Preprocess all Dk for select0/1 queries
32 Preprocess all πk for read and inverse queries



As a technicality, note that we need σ pointers to the bitvectors Ac . If σ is so large
that this becomes an issue, we can concatenate A = A1 . A2 . . . Aσ , and easily translate
the operations used on Ac to operations on the single bitvector A. We leave this as an
exercise to the reader.
*/