#include <catch2/catch_test_macros.hpp>
#include "grid.hpp"

TEST_CASE("Grid test") {
    vector<Point> points = {Point(1, 1), Point(2, 2), Point(3, 3)};
    Grid g(points, 3, 3);
    SECTION("Count") {
        REQUIRE(g.count(1, 3, 1, 3) == 3);
        REQUIRE(g.count(1, 2, 1, 2) == 2);
        REQUIRE(g.count(2, 3, 2, 3) == 2);
        REQUIRE(g.count(1, 1, 1, 1) == 1);
        REQUIRE(g.count(2, 2, 2, 2) == 1);
        REQUIRE(g.count(3, 3, 3, 3) == 1);
        REQUIRE(g.count(1, 1, 2, 2) == 0);
        REQUIRE(g.count(2, 2, 3, 3) == 0);
        REQUIRE(g.count(1, 2, 1, 3) == 2);
        REQUIRE(g.count(2, 3, 1, 3) == 2);
        REQUIRE(g.count(1, 3, 1, 2) == 2);
        REQUIRE(g.count(1, 3, 2, 3) == 2);
        REQUIRE(g.count(1, 2, 2, 3) == 1);
        REQUIRE(g.count(2, 3, 1, 2) == 1);
        REQUIRE(g.count(1, 3, 1, 1) == 1);
    }
    SECTION("Report") {
        vector<Point> res = g.report(1, 3, 1, 3);
        REQUIRE(res.size() == 3);
        REQUIRE(res[0] == Point(1, 1));
        REQUIRE(res[1] == Point(2, 2));
        REQUIRE(res[2] == Point(3, 3));
        res = g.report(1, 2, 1, 2);
        REQUIRE(res.size() == 2);
        REQUIRE(res[0] == Point(1, 1));
        REQUIRE(res[1] == Point(2, 2));
        res = g.report(2, 3, 2, 3);
        REQUIRE(res.size() == 2);
        REQUIRE(res[0] == Point(2, 2));
        REQUIRE(res[1] == Point(3, 3));
        res = g.report(1, 1, 1, 1);
        REQUIRE(res.size() == 1);
        REQUIRE(res[0] == Point(1, 1));
        res = g.report(2, 2, 2, 2);
        REQUIRE(res.size() == 1);
        REQUIRE(res[0] == Point(2, 2));
        res = g.report(3, 3, 3, 3);
        REQUIRE(res.size() == 1);
        REQUIRE(res[0] == Point(3, 3));
        res = g.report(1, 1, 2, 2);
        REQUIRE(res.size() == 0);
        res = g.report(2, 2, 3, 3);
        REQUIRE(res.size() == 0);
        res = g.report(1, 2, 1, 3);
        REQUIRE(res.size() == 2);
        REQUIRE(res[0] == Point(1, 1));
        REQUIRE(res[1] == Point(2, 2));
        res = g.report(2, 3, 1, 3);
        REQUIRE(res.size() == 2);
        REQUIRE(res[0] == Point(2, 2));
        REQUIRE(res[1] == Point(3, 3));
        res = g.report(1, 3, 1, 2);
        REQUIRE(res.size() == 2);
        REQUIRE(res[0] == Point(1, 1));
    }
    
}


/*FILE *input  = fopen(input_filename.c_str(), "rb");
    string filecontent = "";
    char c;
    while (fread(&c, 1, 1, input) == 1) {
        filecontent += c;
    }
    fclose(input);
    cout << "File content: " << filecontent << endl;*/