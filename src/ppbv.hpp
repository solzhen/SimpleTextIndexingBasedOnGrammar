#include <vector>
#include <iostream>
#include <sdsl/bit_vectors.hpp>
using namespace sdsl;
using namespace std;
typedef uint32_t u32; // this special trick will save us 1% code width bro trust me
typedef std::pair<u32, u32> Point;

class PreprocessedBitvector {
private:     
    rrr_vector<127> rrrb;
    rrr_vector<127>::rank_1_type rank_1_;
    rrr_vector<127>::rank_0_type rank_0_;
    rrr_vector<127>::select_1_type select_1_;
    rrr_vector<127>::select_0_type select_0_;
    bit_vector b;
public:    
    PreprocessedBitvector(bit_vector &bv);
    PreprocessedBitvector();
    void preprocess();
    u32 rank_1(u32 i);
    u32 rank_0(u32 i);
    u32 select_1(u32 i);
    u32 select_0(u32 i);
    void printself(u32 l, u32 z);
    void printself();
    u32 operator[](u32 i);
    void bitset(u32 i);
    void bitclear(u32 i);
};
using ppbv = PreprocessedBitvector;