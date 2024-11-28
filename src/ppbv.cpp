#include <vector>
#include <iostream>
#include <sdsl/bit_vectors.hpp>
#include "ppbv.hpp"
using namespace sdsl;
using namespace std;
typedef uint32_t u32; // this special trick will save us 1% code width bro trust me
typedef std::pair<u32, u32> Point;
PreprocessedBitvector::PreprocessedBitvector(bit_vector &bv) {
    rrrb = rrr_vector<127>(bv);   
};
PreprocessedBitvector::PreprocessedBitvector() {
    bit_vector b = bit_vector(1,0);
    rrrb = rrr_vector<127>(b);     
};
void PreprocessedBitvector::preprocess() {
    this->rank_1_ = rrr_vector<127>::rank_1_type(&rrrb);
    this->rank_0_ = rrr_vector<127>::rank_0_type(&rrrb);
    this->select_1_ = rrr_vector<127>::select_1_type(&rrrb);
    this->select_0_ = rrr_vector<127>::select_0_type(&rrrb);
};
u32 PreprocessedBitvector::rank_1(u32 i) {
    if (i == 0) return 0;
    if (i > rrrb.size()) return rank_1(rrrb.size());
    return rank_1_(i);
}
u32 PreprocessedBitvector::rank_0(u32 i) {
    if (i == 0) return 0;
    if (i > rrrb.size()) return rank_0(rrrb.size());
    return rank_0_(i);
}
u32 PreprocessedBitvector::select_1(u32 i) {
    return select_1_(i);
}
u32 PreprocessedBitvector::select_0(u32 i) {
    return select_0_(i);
}
void PreprocessedBitvector::printself(u32 level, u32 z_l) {
    cout << "l:" << ++level << " " << this->rrrb << " z_l:" << z_l << endl;
}
void PreprocessedBitvector::printself() {
    cout << this->rrrb << endl;
}
u32 PreprocessedBitvector::operator[](u32 index) {
    return rrrb[index];
}
/* void PreprocessedBitvector::bitset(u32 i) {
    rrrb[i] = 1;
};
void PreprocessedBitvector::bitclear(u32 i) {
    rrrb[i] = 0;
}; */