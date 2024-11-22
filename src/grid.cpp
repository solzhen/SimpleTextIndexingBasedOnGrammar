#include <string>
#include <vector>
#include <queue>
#include <array>
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/bit_vectors.hpp>
#include <algorithm>
#include <utility> // For using std::pair
#include <unordered_set>
#include <iostream>
#include <cstdint>

#include "grid.hpp"

using namespace sdsl;
using namespace std;

typedef uint32_t u32; // this special trick will save us 1% code width bro trust me
typedef std::pair<u32, u32> Point;

bool sortByX(const Point& p1, const Point& p2) {
    return p1.first < p2.first;
}

u32 Grid::count(u32 x_1, u32 x_2, u32 y_1, u32 y_2, u32 l, u32 a, u32 b) {
    // boundaries
    if (x_1 <= 0) x_1 = 1;
    if (x_2 > c) x_2 = c;
    if (y_1 <= 0) y_1 = 1;
    if (y_2 > r) y_2 = r;
    // negatively sized ranges
    if (x_1 > x_2) return 0;
    if (y_1 > y_2) return 0;
    // empty intersection of [a,b] and [y_1, y_2]
    if (b < y_1 || a > y_2) return 0;
    // [a,b] is a subset of [y_1,y_2]
    if (a >= y_1 && b <= y_2) return x_2 - x_1 + 1;
    u32 xl_1 = wt[l-1].rank_0(x_1 - 1) + 1;
    u32 xl_2 = wt[l-1].rank_0(x_2);
    u32 xr_1 = wt.offset(l-1) + x_1 - xl_1 + 1;
    u32 xr_2 = wt.offset(l-1) + x_2 - xl_2;
    u32 m = (a + b)/2;
    return  count(xl_1, xl_2, y_1, y_2, l+1, a, m) +
            count(xr_1, xr_2, y_1, y_2, l+1, m+1, b);
}
vector<Point> Grid::report(u32 x_1, u32 x_2, u32 y_1, u32 y_2, u32 l, u32 a, u32 b) {
    //std::cout << "report(" << x_1 << "," << x_2 << "," << y_1 << "," << y_2 << "," << l << "," << a << "," << b << ")" << endl;
    std::vector<Point> emptyVec;
    if (x_1 > x_2) {return emptyVec ;}
    if (b < y_1 || a > y_2) {return emptyVec ;}
    if (a >= y_1 && b <= y_2) {
        vector<Point> v1;
        for (u32 i = x_1; i <= x_2; i++) {
            u32 x = outputx(l,i);
            u32 y = outputy(l,a,b,i);
            v1.push_back(Point(x,y));
        }
        return v1;
    }
    u32 xl_1 = wt[l-1].rank_0(x_1 - 1) + 1;
    u32 xl_2 = wt[l-1].rank_0(x_2);
    u32 xr_1 = wt.offset(l-1) + x_1 - xl_1 + 1;
    u32 xr_2 = wt.offset(l-1) + x_2 - xl_2;
    u32 m = (a + b)/2;

    vector<Point> v1 = report(xl_1, xl_2, y_1, y_2, l+1, a, m);
    vector<Point> v2 = report(xr_1, xr_2, y_1, y_2, l+1, m+1, b);
    vector<Point> v3;
    for (const Point& p : v1) 
        v3.push_back(p);
    for (const Point& p : v2) 
        v3.push_back(p);
    return v3;
}
u32 Grid::outputx(u32 l, u32 i) {
    //cout << "outputx(" << l << "," << i << ")" << endl;
    while (l>1) {
        //wt[l-1].printself();
        l = l-1;
        if (i <= wt.offset(l-1)) {
            i = wt[l-1].select_0(i) +1;
        } else {
            i = wt[l-1].select_1(i - wt.offset(l-1)) +1;
        }
    }
    return i;
}
u32 Grid::outputy(u32 l, u32 a, u32 b, u32 i) {
    //std::cout << "outputy(" << l << "," << a << "," << b << "," << i << ")" << endl;
    while (a != b) {
        if (wt[l-1][i-1] == 0) {            
            i = wt[l-1].rank_0(i);
            b = (a+b)/2;
        } else {
            i = wt.offset(l-1) + wt[l-1].rank_1(i);
            a = (a+b)/2 + 1;
        }
        l++;
    }
    return a;
}
Grid::Grid(const string& fn) : filename(fn) {
    vector<Point> points = readPointsFromFile(filename, c, r); // symbol range is = [1, rows]
    n = points.size();
    bit_vector b(n+c);
    // We first sort the pairs by x-coord
    std::sort(points.begin(), points.end(), sortByX);
    //writePointsToFile(filename, c, r, points);
    // build map bitvector
    u32 last_x_coord = 0;
    int i = 0;
    for (const auto& point : points) {        
        if (point.first > last_x_coord) { // x-coord increases
            u32 d = point.first - last_x_coord; // 1, or if we skipped a few columns we gotta add those 1s
            for (u32 a=0; a < d; a++) b[i+a] = 1;
            i = i + d;
            last_x_coord = point.first;
        } i++;            
    }
    bv = PreprocessedBitvector(b);
    bv.preprocess();
    writeYToFile(filename, points); //pack Y values, may want to write c and r 
    vector<u32> yvalues = readIntegersFromFile(filename);      
    wt = WaveletMatrix(yvalues, r);
}
Grid::Grid(std::vector<Point> &points, u32 columns, u32 rows) {
    c = columns;
    r = rows;
    n = points.size();
    bit_vector b(n+c);
    // We first sort the pairs by x-coord
    std::sort(points.begin(), points.end(), sortByX);
    // build map bitvector
    u32 last_x_coord = 0;
    int i = 0;
    for (const auto& point : points) {        
        if (point.first > last_x_coord) { // x-coord increases
            u32 d = point.first - last_x_coord; // 1, or if we skipped a few columns we gotta add those 1s
            for (u32 a=0; a < d; a++) b[i+a] = 1;
            i = i + d;
            last_x_coord = point.first;
        } i++;            
    }
    bv = PreprocessedBitvector(b);
    bv.preprocess();
    vector<u32> yvalues(n);
    for (u32 i = 0; i < n; i++) {
        yvalues[i] = points[i].second;
    }  
    wt = WaveletMatrix(yvalues, r);
};

u32 Grid::count(u32 x_1, u32 x_2, u32 y_1, u32 y_2) {        
    return count(x_1, x_2, y_1, y_2, 1, 1, this->r);
}
vector<Point> Grid::report(u32 x_1, u32 x_2, u32 y_1, u32 y_2) {
    return report(x_1, x_2, y_1, y_2, 1, 1, this->r);
}
void Grid::printself() {
    this->wt.printself();
}