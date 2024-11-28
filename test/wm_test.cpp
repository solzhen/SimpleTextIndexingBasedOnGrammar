#include <catch2/catch_test_macros.hpp>
#include "wavelet_matrix.hpp"

TEST_CASE("WaveletMatrix access operation", "[wavelet_matrix]") {
    std::vector<uint32_t> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    WaveletMatrix wm = WaveletMatrix(vec, 9);
    REQUIRE(wm.access(0) == 1);
    REQUIRE(wm.access(1) == 2);
    REQUIRE(wm.access(2) == 3);
    REQUIRE(wm.access(3) == 4);
    REQUIRE(wm.access(4) == 5);
    REQUIRE(wm.access(5) == 6);
    REQUIRE(wm.access(6) == 7);
    REQUIRE(wm.access(7) == 8);
    REQUIRE(wm.access(8) == 9);
    
}