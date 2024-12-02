#include <catch2/catch_test_macros.hpp>
#include "wavelet_matrix.hpp"

TEST_CASE("WaveletMatrix operations 1 to 9 vector", "[wavelet_matrix]") {
    std::vector<uint32_t> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    REQUIRE(vec.size() == 9);
    WaveletMatrix wm = WaveletMatrix(vec, 9);    
    //wm.printself();
    SECTION("Access") {
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
    SECTION("Rank (c, i) in the sequence") {  
        // Rank(c, i) is the number of occurences of c
        // in the sequence up until i (not including i)
        // i is a 0-indexed position
        REQUIRE(wm.rank(1, 0) == 0);
        REQUIRE(wm.rank(1, 1) == 1);
        REQUIRE(wm.rank(1, 9) == 1);
        REQUIRE(wm.rank(1, 10) == 1);
        REQUIRE(wm.rank(2, 0) == 0);
        REQUIRE(wm.rank(2, 1) == 0);
        REQUIRE(wm.rank(2, 2) == 1);
        REQUIRE(wm.rank(2, 10) == 1);
        REQUIRE(wm.rank(3, 2) == 0);
        REQUIRE(wm.rank(3, 3) == 1);
        REQUIRE(wm.rank(3, 10) == 1);
        REQUIRE(wm.rank(4, 3) == 0);
        REQUIRE(wm.rank(4, 4) == 1);
        REQUIRE(wm.rank(4, 10) == 1);
        REQUIRE(wm.rank(5, 4) == 0);
        REQUIRE(wm.rank(5, 5) == 1);
        REQUIRE(wm.rank(5, 10) == 1);
        REQUIRE(wm.rank(6, 5) == 0);
        REQUIRE(wm.rank(6, 6) == 1);
        REQUIRE(wm.rank(6, 10) == 1);
        REQUIRE(wm.rank(7, 6) == 0);
        REQUIRE(wm.rank(7, 7) == 1);
        REQUIRE(wm.rank(7, 10) == 1);
        REQUIRE(wm.rank(8, 7) == 0);
        REQUIRE(wm.rank(8, 8) == 1);
        REQUIRE(wm.rank(8, 10) == 1);
        REQUIRE(wm.rank(9, 8) == 0);
        REQUIRE(wm.rank(9, 9) == 1);
        REQUIRE(wm.rank(9, 10) == 1);
    }    
    SECTION("Select (c, j)") {
        // Select(c, j) is the 0-indexed position of the j-th occurence of c
        REQUIRE(wm.select(1, 1) == 0);
        REQUIRE(wm.select(2, 1) == 1);
        REQUIRE(wm.select(3, 1) == 2);
        REQUIRE(wm.select(4, 1) == 3);
        REQUIRE(wm.select(5, 1) == 4);
        REQUIRE(wm.select(6, 1) == 5);
        REQUIRE(wm.select(7, 1) == 6);
        REQUIRE(wm.select(8, 1) == 7);
        REQUIRE(wm.select(9, 1) == 8);
    }
}

TEST_CASE("Wavelet Matric operation for sequence {1,1,1,1,1,1,2,2,2,2,2}", "[wavelet_matrix]") {
    std::vector<uint32_t> vec = {1,1,1,1,1,1,2,2,2,2,2};
    REQUIRE(vec.size() == 11);
    WaveletMatrix wm = WaveletMatrix(vec, 2);
    //wm.printself();
    SECTION("Access") {
        REQUIRE(wm.access(0) == 1);
        REQUIRE(wm.access(1) == 1);
        REQUIRE(wm.access(2) == 1);
        REQUIRE(wm.access(3) == 1);
        REQUIRE(wm.access(4) == 1);
        REQUIRE(wm.access(5) == 1);
        REQUIRE(wm.access(6) == 2);
        REQUIRE(wm.access(7) == 2);
        REQUIRE(wm.access(8) == 2);
        REQUIRE(wm.access(9) == 2);
        REQUIRE(wm.access(10) == 2);
    }
    SECTION("Rank (c, i) in the sequence") {
        REQUIRE(wm.rank(1, 0) == 0);
        REQUIRE(wm.rank(1, 1) == 1);
        REQUIRE(wm.rank(1, 2) == 2);
        REQUIRE(wm.rank(1, 3) == 3);
        REQUIRE(wm.rank(1, 4) == 4);
        REQUIRE(wm.rank(1, 5) == 5);
        REQUIRE(wm.rank(1, 6) == 6);
        REQUIRE(wm.rank(1, 7) == 6);
        REQUIRE(wm.rank(1, 8) == 6);
        REQUIRE(wm.rank(1, 9) == 6);
        REQUIRE(wm.rank(1, 10) == 6);
        REQUIRE(wm.rank(2, 0) == 0);
        REQUIRE(wm.rank(2, 5) == 0);
        REQUIRE(wm.rank(2, 6) == 0);
        REQUIRE(wm.rank(2, 7) == 1);
        REQUIRE(wm.rank(2, 8) == 2);
        REQUIRE(wm.rank(2, 9) == 3);
        REQUIRE(wm.rank(2, 10) == 4);
        REQUIRE(wm.rank(2, 11) == 5);
    }
    SECTION("Select (c, j)") {    
        REQUIRE(wm.select(1, 1) == 0);
        REQUIRE(wm.select(1, 2) == 1);
        REQUIRE(wm.select(1, 3) == 2);
        REQUIRE(wm.select(1, 4) == 3);
        REQUIRE(wm.select(1, 5) == 4);
        REQUIRE(wm.select(1, 6) == 5);
        REQUIRE(wm.select(2, 1) == 6);
        REQUIRE(wm.select(2, 2) == 7);
        REQUIRE(wm.select(2, 3) == 8);
        REQUIRE(wm.select(2, 4) == 9);
        REQUIRE(wm.select(2, 5) == 10);
        REQUIRE(wm.select(2, 6) == wm.size());
        REQUIRE(wm.select(3, 1) == -1);
    }
}
TEST_CASE("Matrix for vector ={1 2 3 4 5 6 7 8 9 10}", "[wavelet_matrix]") {
    vector<uint32_t> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    REQUIRE(vec.size() == 10);
    WaveletMatrix wm = WaveletMatrix(vec, 10);
    //wm.printself();
    SECTION("Access") {
        REQUIRE(wm.access(0) == 1);
        REQUIRE(wm.access(1) == 2);
        REQUIRE(wm.access(2) == 3);
        REQUIRE(wm.access(3) == 4);
        REQUIRE(wm.access(4) == 5);
        REQUIRE(wm.access(5) == 6);
        REQUIRE(wm.access(6) == 7);
        REQUIRE(wm.access(7) == 8);
        REQUIRE(wm.access(8) == 9);
        REQUIRE(wm.access(9) == 10);
    }
}
TEST_CASE("Matrix for vector ={10 1 2 3 4 5 6 7 8 9}", "[wavelet_matrix]") {
    vector<uint32_t> vec = {10, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    REQUIRE(vec.size() == 10);
    WaveletMatrix wm = WaveletMatrix(vec, 10);
    //wm.printself();
    SECTION("Access") {
        REQUIRE(wm.access(0) == 10);
        REQUIRE(wm.access(1) == 1);
        REQUIRE(wm.access(2) == 2);
        REQUIRE(wm.access(3) == 3);
        REQUIRE(wm.access(4) == 4);
        REQUIRE(wm.access(5) == 5);
        REQUIRE(wm.access(6) == 6);
        REQUIRE(wm.access(7) == 7);
        REQUIRE(wm.access(8) == 8);
        REQUIRE(wm.access(9) == 9);
    }
}
TEST_CASE("Matrix for vector ={10 1 2 3 11 4 5 6 7 8 9}", "[wavelet_matrix]") {
    vector<uint32_t> vec = {10, 1, 2, 3, 11, 4, 5, 6, 7, 8, 9};
    REQUIRE(vec.size() == 11);
    WaveletMatrix wm = WaveletMatrix(vec, 11);
    //wm.printself();
    SECTION("Access") {
        REQUIRE(wm.access(0) == 10);
        REQUIRE(wm.access(1) == 1);
        REQUIRE(wm.access(2) == 2);
        REQUIRE(wm.access(3) == 3);
        REQUIRE(wm.access(4) == 11);
        REQUIRE(wm.access(5) == 4);
        REQUIRE(wm.access(6) == 5);
        REQUIRE(wm.access(7) == 6);
        REQUIRE(wm.access(8) == 7);
        REQUIRE(wm.access(9) == 8);
        REQUIRE(wm.access(10) == 9);
    }
}
