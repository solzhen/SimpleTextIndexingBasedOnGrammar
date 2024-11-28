#include <catch2/catch_test_macros.hpp>
#include "ppbv.hpp"

TEST_CASE("Preprocessed bitvector", "[ppbv]") {
    bit_vector bv(10, 0);
    bv[0] = 1;
    bv[1] = 0;
    bv[2] = 1;
    bv[3] = 0;
    bv[4] = 1;
    bv[5] = 0;
    bv[6] = 1;
    bv[7] = 0;
    bv[8] = 1;
    bv[9] = 0;
    ppbv pbv = ppbv(bv);
    pbv.preprocess();
    REQUIRE(pbv.size() == 10);
    SECTION("Rank 1") {
        REQUIRE(pbv.rank_1(0) == 0);
        REQUIRE(pbv.rank_1(1) == 1);
        REQUIRE(pbv.rank_1(2) == 1);
        REQUIRE(pbv.rank_1(3) == 2);
        REQUIRE(pbv.rank_1(4) == 2);
        REQUIRE(pbv.rank_1(5) == 3);
        REQUIRE(pbv.rank_1(6) == 3);
        REQUIRE(pbv.rank_1(7) == 4);
        REQUIRE(pbv.rank_1(8) == 4);
        REQUIRE(pbv.rank_1(9) == 5);
        REQUIRE(pbv.rank_1(10) == 5);
    }
    SECTION("Rank 0") {
        REQUIRE(pbv.rank_0(0) == 0);
        REQUIRE(pbv.rank_0(1) == 0);
        REQUIRE(pbv.rank_0(2) == 1);
        REQUIRE(pbv.rank_0(3) == 1);
        REQUIRE(pbv.rank_0(4) == 2);
        REQUIRE(pbv.rank_0(5) == 2);
        REQUIRE(pbv.rank_0(6) == 3);
        REQUIRE(pbv.rank_0(7) == 3);
        REQUIRE(pbv.rank_0(8) == 4);
        REQUIRE(pbv.rank_0(9) == 4);
        REQUIRE(pbv.rank_0(10) == 5);
    }
    SECTION("Select 1") {
        REQUIRE(pbv.select_1(1) == 0);
        REQUIRE(pbv.select_1(2) == 2);
        REQUIRE(pbv.select_1(3) == 4);
        REQUIRE(pbv.select_1(4) == 6);
        REQUIRE(pbv.select_1(5) == 8);
        REQUIRE(pbv.select_1(6) == 10);
    }
    SECTION("Select 0") {
        REQUIRE(pbv.select_0(1) == 1);
        REQUIRE(pbv.select_0(2) == 3);
        REQUIRE(pbv.select_0(3) == 5);
        REQUIRE(pbv.select_0(4) == 7);
        REQUIRE(pbv.select_0(5) == 9);
        REQUIRE(pbv.select_0(6) == 10);
    }
}