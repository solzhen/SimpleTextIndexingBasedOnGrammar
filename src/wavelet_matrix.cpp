#include <vector>
#include <iostream>
#include <cstdint>
#include <string>
#include <cmath>
#include <sdsl/bit_vectors.hpp>
#include "wavelet_matrix.hpp"
#include "debug_config.hpp"

using namespace sdsl;
using namespace std;

typedef uint32_t u32; // this special trick will save us 1% code width bro trust me
typedef std::pair<u32, u32> Point;

using ppbv = PreprocessedBitvector;

void WaveletMatrix::build(vector<u32>& S, u32 n, u32 sigma) {
    vector<u32> S_hat(n);
    bit_vector M(n, 0);
    bit_vector M_hat(n, 0);
    u32 m = sigma;
    //int x,y; x = 15846;
    //cout << ceil(log2(sigma))-1 << endl;
    for (u32 l = 0; l <= ceil(log2(sigma))-1; l++) {    
        //cout << "l: " << l << " n: " << n << " m:" << m << flush;
        u32 z_l = 0;
        bit_vector B_l(n, 0);
        for (u32 i = 0; i < n; i++) {
            //if (i == x) cout << " i: " << i << " S[i]: " << S[i] << " M[i]: " << M[i] << endl;
            if (S[i] <= (m - M[i] + 1) / 2) { 
                B_l[i] = 0;
                z_l++;
            } else {
                B_l[i] = 1;
                S[i] = S[i] - (m - M[i] + 1) / 2;
            }
        }
        bm.push_back(ppbv(B_l));
        z.push_back(z_l);
        if (l < ceil(log2(sigma)) - 1) {
            u32 p_l = -1; // max value + 1 = 0
            u32 p_r = z[l] - 1;
            u32 p;
            int n_ = n;
            for (u32 i = 0; i < n_; i++) {
                u32 b = bm[l][i];
                if (b == 0) {                    
                    p_l ++;  
                    p = p_l;
                } else {                    
                    p_r ++;
                    p = p_r;              
                }                
                //if (i==x) cout << "i: " << i << " p: " << p << endl;// <---------
                //if (i==x) y = p;// <---------
                S_hat[p] = S[i];
                if (m % 2 == b) {
                    M_hat[p] = b;
                } else {
                    M_hat[p] = M[i];
                }
                if ((m+1)/2==2 && M_hat[p] == 1) {
                    n = n-1;;
                }
            }
            //x = y - (n_ - n); // <---------            
            swap(S, S_hat);
            swap(M, M_hat);
            m = (m+1)/2;
        }
    }
    S.clear();
    S.shrink_to_fit();
}

WaveletMatrix::WaveletMatrix(vector<u32>& s, u32 sigma) {
    this->sigma = sigma;
    build(s, s.size(), sigma);
    for (u32 i = 0; i < bm.size(); i++)
        bm[i].preprocess();
};

WaveletMatrix::WaveletMatrix() {
    this->sigma=1;
    vector<u32> p = {0};
    build(p, 1, sigma);
}

u32 WaveletMatrix::access(u32 i_) {    
    u32 i = i_;
    u32 l = 1;
    u32 a = 1;
    u32 b = sigma;
    while (a != b) {
        if (DEBUG) cout << "i: " << i << flush;
        if (DEBUG) cout << " a:" << a << ", b:" << b << endl; 
        if (bm[l-1][i] == 0) {         
            i = bm[l-1].rank_0(i);
            b = ((a + b) / 2);
        } else {
            i = z[l-1] + bm[l-1].rank_1(i);
            a = ((a + b) / 2) + 1;
        }
        l++;
    }
    return a;
}
u32 WaveletMatrix::rank(u32 c, u32 i_) {
    if (c < 1 || c > sigma) return 0;
    u32 p = 0;
    u32 i = i_;
    u32 l = 1;
    u32 a = 1; // first symbol
    u32 b = sigma; // last symbol
    while (a != b) {      
        if (c <= (a+b)/2) { 
            i = bm[l-1].rank_0(i);
            b = ((a+b)/2);
            p = bm[l-1].rank_0(p);
        } else {
            i = z[l-1] + bm[l-1].rank_1(i);
            a = (a+b)/2 + 1;
            p = z[l-1] + bm[l-1].rank_1(p);
        }
        l++;
    }
    return i - p;
}
u32 WaveletMatrix::select(u32 c, u32 j) {
    if (c < 1 || c > sigma) return -1;
    return select(1,0,1,sigma,c,j) - 1;
}

u32 WaveletMatrix::select(u32 l, u32 p, u32 a, u32 b, u32 c, u32 j_) {
    u32 j = j_;
    if (a == b) {
        return p + j;
    }
    if (c <= ((a+b)/2)) {
        j = select(l+1, bm[l-1].rank_0(p), a, ((a+b)/2), c, j);
        return bm[l-1].select_0(j)+1;
    } else {
        j = select(l+1, z[l-1]+bm[l-1].rank_1(p), ((a+b)/2)+1, b, c, j);
        return bm[l-1].select_1(j-z[l-1])+1;
    }
}

void WaveletMatrix::printself() {
    int l = 0;
    for (u32 i = 0; i < this->bm.size(); i++) {
        bm[i].printself(l, z[l]);
        l++;
    }

}

ppbv WaveletMatrix::operator[](u32 level) {
    return this->bm[level];
}

u32 WaveletMatrix::offset(u32 level) {
    return z[level];
}