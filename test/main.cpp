#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#include <sdsl/int_vector.hpp>

TEST_CASE("sdsl::int_vector basic operations", "[int_vector]") {
    sdsl::int_vector<1> vec(10); // Create a 10-element vector
    vec[0] = 1;
    vec[1] = 0;
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 0);
    REQUIRE(vec.size() == 10);
}

