#include <catch2/catch_test_macros.hpp>
#include "sequences.hpp"

TEST_CASE("Sequence basic operations on 1,0 alternating sequence", "[sequence]") {
    int_vector<> vec({1, 0, 1, 0, 1, 0, 1, 0, 1, 0});
    ARSSequence seq = ARSSequence(vec, 2);
    REQUIRE(seq.size() == 10);
    REQUIRE(seq.access(0) == 1);
    REQUIRE(seq.access(1) == 0);
    REQUIRE(seq.access(2) == 1);
    REQUIRE(seq.access(3) == 0);
    REQUIRE(seq.access(4) == 1);
    REQUIRE(seq.access(5) == 0);
    REQUIRE(seq.access(6) == 1);
    REQUIRE(seq.access(7) == 0);
    REQUIRE(seq.access(8) == 1);
    REQUIRE(seq.access(9) == 0);
    SECTION("Select (c j)") {
        REQUIRE(seq.select(1, 1) == 0);
        REQUIRE(seq.select(1, 2) == 2);
        REQUIRE(seq.select(1, 3) == 4);
        REQUIRE(seq.select(1, 4) == 6);
        REQUIRE(seq.select(1, 5) == 8);
        REQUIRE(seq.select(0, 1) == 1);
        REQUIRE(seq.select(0, 2) == 3);
        REQUIRE(seq.select(0, 3) == 5);
        REQUIRE(seq.select(0, 4) == 7);
        REQUIRE(seq.select(0, 5) == 9);
    }
    SECTION("Rank (c, i)") {
        REQUIRE(seq.rank(1, 0) == 1);
        REQUIRE(seq.rank(1, 1) == 1);
        REQUIRE(seq.rank(1, 2) == 2);
        REQUIRE(seq.rank(1, 3) == 2);
        REQUIRE(seq.rank(1, 4) == 3);
        REQUIRE(seq.rank(1, 5) == 3);
        REQUIRE(seq.rank(1, 6) == 4);
        REQUIRE(seq.rank(1, 7) == 4);
        REQUIRE(seq.rank(1, 8) == 5);
        REQUIRE(seq.rank(1, 9) == 5);
        REQUIRE(seq.rank(0, 0) == 0);
        REQUIRE(seq.rank(0, 1) == 1);
        REQUIRE(seq.rank(0, 2) == 1);
        REQUIRE(seq.rank(0, 3) == 2);
        REQUIRE(seq.rank(0, 4) == 2);
        REQUIRE(seq.rank(0, 5) == 3);
        REQUIRE(seq.rank(0, 6) == 3);
        REQUIRE(seq.rank(0, 7) == 4);
        REQUIRE(seq.rank(0, 8) == 4);
        REQUIRE(seq.rank(0, 9) == 5);
    }
}