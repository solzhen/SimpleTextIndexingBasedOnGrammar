#include <vector>
#include <iostream>
#include <cstdint>
#include <string>
#include <cmath>
#include <sdsl/bit_vectors.hpp>
#include "wavelet_matrix.hpp"

using namespace sdsl;
using namespace std;

typedef uint32_t u32; // this special trick will save us 1% code width bro trust me
typedef std::pair<u32, u32> Point;

struct preprocessed_bitvector {
    bit_vector b;
    rrr_vector<127> rrrb;
    rrr_vector<127>::rank_1_type rank_1;
    rrr_vector<127>::rank_0_type rank_0;
    rrr_vector<127>::select_1_type select_1;
    rrr_vector<127>::select_0_type select_0;
    preprocessed_bitvector(bit_vector bv) {
        b = bv;
        rrrb = rrr_vector<127>(b);
        rank_1 = rrr_vector<127>::rank_1_type(&rrrb);
        rank_0 = rrr_vector<127>::rank_0_type(&rrrb);
        select_1 = rrr_vector<127>::select_1_type(&rrrb);
        select_0 = rrr_vector<127>::select_0_type(&rrrb);
    };
};
using ppbv = preprocessed_bitvector;

void WaveletMatrix::build(vector<u32>& S, u32 n, u32 sigma) {
    vector<u32> S_hat(n);
    bit_vector M(n, 0);
    bit_vector M_hat(n, 0);
    u32 m = sigma;
    for (u32 l = 1; l <= ceil(log2(sigma)); l++) {
        u32 z_l = 0;
        bit_vector B_l(n, 0);
        for (u32 i =0; i < n; i++) {
            if (S[i] <= ceil((m - M[i]) / 2) ) {
                B_l[i] = 0;
                z_l++;
            } else {
                B_l[i] = 1;
                S[i] = S[i] - ceil((m - M[i]) / 2);
            }
        }
        bm.push_back(ppbv(B_l));
        z.push_back(z_l);
        if (l < ceil(log2(sigma))) {
            u32 p_l = 0;
            u32 p_r = z_l;
            u32 p;
            for (u32 i = 0; i < n; i++) {
                if (bm[l - 1].b[i] == 0) {
                    p = p_l;
                    p_l ++;                        
                } else {
                    p = p_r;
                    p_r ++;                        
                }
                S_hat[p] = S[i];
                if (m % 2 == bm[l - 1].b[i]) {
                    M_hat[p] = bm[l - 1].b[i];
                } else {
                    M_hat[p] = M[i];
                }
                if (ceil(m/2)==2 && M_hat[p] == 1) {
                    n = n-1;
                }
            }
            swap(S, S_hat);
            m = ceil(m/2);
        }
    }
    S.clear();
    S.shrink_to_fit();       
}

WaveletMatrix::WaveletMatrix(vector<u32>& s, u32 sigma) {
    this->sigma = sigma;
    build(s, s.size(), sigma);
};
u32 WaveletMatrix::access(u32 i_) {
    u32 i = i_;
    u32 l = 1;
    u32 a = 1;
    u32 b = sigma;
    while (a != b) {
        if (bm[l - 1].b[i] == 0) {
            i = bm[l-1].rank_0(i);
            b = floor((a + b) / 2);
        } else {
            i = z[l-1] + bm[l-1].rank_1(i);
            a = floor((a + b) / 2) + 1;
        }
        l++;
    }
    return a;
}
u32 WaveletMatrix::rank(u32 i_, u32 c) {
    u32 i = i_;
    u32 p = 0;
    u32 l = 1;
    u32 a = 1;
    u32 b = sigma;
    while (a != b) {
        if (c <= floor((a+b)/2)) {
            i = bm[l-1].rank_0(i);
            p = bm[l-1].rank_0(p);
            b = floor((a + b) / 2); 
        } else {
            i = z[l-1] + bm[l-1].rank_1(i);
            p = z[l-1] + bm[l-1].rank_1(p);
            a = floor((a + b) / 2) + 1;
        }
        l++;
    }
    return i - p;
}
u32 WaveletMatrix::select(u32 j, u32 c) {
    return select(1,0,1,sigma,c,j);
}

u32 WaveletMatrix::select(u32 l, u32 p, u32 a, u32 b, u32 c, u32 j_) {
    u32 j = j_;
    if (a == b) {
        return p + j;
    }
    if (c <= floor((a+b)/2)) {
        j = select(l+1, bm[l-1].rank_0(p), a, floor((a+b)/2), c, j);
        return bm[l-1].select_0(j);
    } else {
        j = select(l+1, z[l-1]+bm[l-1].rank_1(p), a, floor((a+b)/2)+1, c, j);
        return bm[l-1].select_1(j-z[l-1]);
    }
}