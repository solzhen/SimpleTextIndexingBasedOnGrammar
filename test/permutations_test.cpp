#include <catch2/catch_test_macros.hpp>
#include "permutations.hpp"

TEST_CASE("Permutation basic operations on pi(i) = (i+1)mod10", "[permutation]") {
    int_vector<> pi({1, 2, 3, 4, 5, 6, 7, 8, 9, 0});
    Permutation perm = Permutation(pi, 3);
    REQUIRE(perm[0] == 1);
    REQUIRE(perm[1] == 2);
    REQUIRE(perm[2] == 3);
    REQUIRE(perm[3] == 4);
    REQUIRE(perm[4] == 5);
    REQUIRE(perm[5] == 6);
    REQUIRE(perm[6] == 7);
    REQUIRE(perm[7] == 8);
    REQUIRE(perm[8] == 9);
    REQUIRE(perm[9] == 0);
    SECTION("Inverse") {
        REQUIRE(perm.inverse(0) == 9);
        REQUIRE(perm.inverse(1) == 0);
        REQUIRE(perm.inverse(2) == 1);
        REQUIRE(perm.inverse(3) == 2);
        REQUIRE(perm.inverse(4) == 3);
        REQUIRE(perm.inverse(5) == 4);
        REQUIRE(perm.inverse(6) == 5);
        REQUIRE(perm.inverse(7) == 6);
        REQUIRE(perm.inverse(8) == 7);
        REQUIRE(perm.inverse(9) == 8);
    }
}
TEST_CASE("Book Example: 9 6 2 4 7 0 10 11 3 5 8 1", "[permutation2]") {
    int_vector<> pi({9, 6, 2, 4, 7, 0, 10, 11, 3, 5, 8, 1});
    Permutation perm = Permutation(pi, 3);
    REQUIRE(perm[0] == 9);
    REQUIRE(perm[1] == 6);
    REQUIRE(perm[2] == 2);
    REQUIRE(perm[3] == 4);
    REQUIRE(perm[4] == 7);
    REQUIRE(perm[5] == 0);
    REQUIRE(perm[6] == 10);
    REQUIRE(perm[7] == 11);
    REQUIRE(perm[8] == 3);
    REQUIRE(perm[9] == 5);
    REQUIRE(perm[10] == 8);
    REQUIRE(perm[11] == 1);
    SECTION("Inverse") {
        REQUIRE(perm.inverse(0) == 5);
        REQUIRE(perm.inverse(1) == 11);
        REQUIRE(perm.inverse(2) == 2);
        REQUIRE(perm.inverse(3) == 8);
        REQUIRE(perm.inverse(4) == 3);
        REQUIRE(perm.inverse(5) == 9);
        REQUIRE(perm.inverse(6) == 1);
        REQUIRE(perm.inverse(7) == 4);
        REQUIRE(perm.inverse(8) == 10);
        REQUIRE(perm.inverse(9) == 0);
        REQUIRE(perm.inverse(10) == 6);
        REQUIRE(perm.inverse(11) == 7);
    }
}