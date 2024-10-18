#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include "grid.hpp"

extern "C" {
    #include "../repairs/repair110811/repair.h"
    #include "../repairs/repair110811/encoder.h"
}

int test(DICT *dict) {

    RULE *rules = dict->rule;
    CODE *comp_seq = dict->comp_seq;

    std::cout << "Number of rules: " << dict->num_rules << std::endl;
    std::cout << "Non terminal Rules: " << dict->num_rules - 257 << std::endl;

    int nt = dict->num_rules - 257;

    std::vector<int> indexMap(nt);
    std::vector<int> reverseIndexMap(nt);

    for (size_t i = 0; i < indexMap.size(); ++i) {
        indexMap[i] = i + 257;
        reverseIndexMap[i] = i + 257;
    }
    /*

    std::sort(indexMap.begin(), indexMap.end(), [&](int a, int b) { return compareRules(rules, a, b); });
    std::cout << "index map completed" << std::endl;

    std::sort(reverseIndexMap.begin(), reverseIndexMap.end(), [&](int a, int b) { return compareRules(rules, a, b, true); });
    std::cout << "reverse index map completed" << std::endl;

    //print indexMap
    for (int i = 0; i < indexMap.size(); i++) {
        std::cout << indexMap[i] << " ";
    } std::cout << std::endl;

    //print reverseIndexMap
    for (int i = 0; i < reverseIndexMap.size(); i++) {
        std::cout << reverseIndexMap[i] << " ";
    } std::cout << std::endl;    

    std::vector<Point> points(nt);
    uint j, k;
    for (int i = 0; i < indexMap.size(); i++) {
        j = indexMap[i]; // rule index
        std::cout << "j: " << j << std::endl;
        k = std::distance(reverseIndexMap.begin(), std::find(reverseIndexMap.begin(), reverseIndexMap.end(), j));
        points[i] = Point(i, k);
    }

    printPoints(points);
    std::cout << std::endl;

    

    //writePointsToFile("test_grid.bin", rules.size(), rules.size(), points);

    return 0;*/
}

/**
 * Now we gotta create a file
 *
 * NOW LET'S MAKE THE GRID
 * OUR GRID CLASS REQUIRES A FILE WITH THE FORMAT C (columns) R (rows) POINTS
 * so we must create the file
 *
 * each column has a point so we go column by column
 *
 * first, to know which row intersects first column, we look at our lexicographically sorted list
 *
 * if the number in first position (zeroth position) is X, then we look for X in the reverse-lexico list and return the index
 *
 * and so on:
 *  for each ith column i
 *    j <- rule index <- indexMap[i]
 *    k <- search(reverseIndexMap, j)
 *    row <- k
 *
 *
 */
