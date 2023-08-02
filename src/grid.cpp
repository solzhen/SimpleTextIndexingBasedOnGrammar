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

#include "file_handler.hpp"
#include "printers.hpp"
#include "grid.hpp"

using namespace sdsl;
using namespace std;

typedef uint32_t u32; // this special trick will save us 1% code width bro trust me
typedef std::pair<u32, u32> Point;

bool sortByX(const Point& p1, const Point& p2) {
    return p1.first < p2.first;
}



/// @brief A grid representation. Input file is a binary file of a sequence
/// of 4 bytes integers with the format 
/// {columns rows x_1 y_1 x_2 y_2 ... x_n y_n}
struct Grid {
    string filename; // external file   
    u32 c; // number of columns
    u32 r; // number of rows
    u32 n; // nubmer of points
    vector<Point> points; // Grid points
    wt_blcd_int<> wt; // Wavelet tree
    bit_vector b; // mapping bitvector
    rrr_vector<127> rrrb;
    rrr_vector<127>::rank_1_type rank_1;
    rrr_vector<127>::rank_0_type rank_0;
    rrr_vector<127>::select_1_type select_1;
    rrr_vector<127>::select_0_type select_0;
    vector<u32> symbols;
    // Constructor
    Grid(const string& fn) : filename(fn) {
        points = readPointsFromFile(filename, c, r, symbols); // symbol range is = [1, rows]
        sort(symbols.begin(), symbols.end());
        n = points.size();
        b = bit_vector(n + c);
        // We first sort the pairs by x-coord
        std::sort(points.begin(), points.end(), sortByX);
        writePointsToFile(filename, c, r, points);
        // build map bitvector
        u32 last_x_coord = 0;
        int i = 0;
        for (const auto& point : points) {        
            if (point.first > last_x_coord) { // x-coord increases
                u32 d = point.first - last_x_coord; // 1, or if we skipped a few columns we gotta add those 1s
                for (u32 a=0; a < d; a++) b[i + a] = 1;
                i = i + d;
                last_x_coord = point.first;
            } i++;            
        }
        rrrb = rrr_vector<127>(b);
        rank_1 = rrr_vector<127>::rank_1_type(&rrrb);
        rank_0 = rrr_vector<127>::rank_0_type(&rrrb);
        select_1 = rrr_vector<127>::select_1_type(&rrrb);
        select_0 = rrr_vector<127>::select_0_type(&rrrb);
        writeYToFile(filename, points); //pack Y values, may want to write c and r        
        construct(wt, filename, 4); //WT sequence
    };
/*     /// @brief returns the number of points lying on the rectangle
    /// [x_1 , x_2 ] × [y_1 , y_2 ] of the grid,
    /// @param x_1 
    /// @param x_2 
    /// @param y_1 
    /// @param y_2 
    /// @return 
    u32 count(u32 x_1, u32 x_2, u32 y_1, u32 y_2) {
        return count(x_1, x_2, y_1, y_2, wt.root(), 1, symbols.size());
    };
    /// @brief Range [x_1, x_2] of level l, inside a segment of S_l that represents
    /// symbols in [a, b] and symbol range [y_1 , y_2].
    /// @return The number of symbols in S_l [x_1 , x_2] belonging to [y_1 , y_2].
    u32 count(u32 x_1, u32 x_2, u32 y_1, u32 y_2, u32 v, u32 a_i, u32 b_i) {
        u32 a = symbols[a_i - 1];
        u32 b = symbols[b_i - 1];

        cout << "symbol range: [" << to_string(a) << "," << to_string(b) << "]" << endl;
        auto seq_vec = wt.seq(v);
        cout << "NodeSeq : ";
        for (auto it = seq_vec.begin(); it!=seq_vec.end(); ++it) {
            u32 value = *it;
            cout << to_string(value) << " ";
        } cout << endl;
        cout << "x_1:" << to_string(x_1) << ", x_2:" << to_string(x_2) << endl;
    
        if (x_1 > x_2) return 0;
        if (b < y_1 || y_2 < a) return 0;
        if (a >= y_1 && b <= y_2) return x_2 - x_1 + 1;

        bit_vector v_bv = wt.bit_vec(v);
        rank_support_v<0> rb0(&v_bv);
        rank_support_v<> rb1(&v_bv);

        auto vs = wt.expand(v); // get the children of this (v) node
        u32 m = (a_i + b_i)/2;
        u32 a_l = a_i;
        u32 b_l = m;
        u32 a_r = m + 1;
        u32 b_r = b_i;             
        uint64_t x_1_l = rb0(x_1 - 1) + 1;
        uint64_t x_2_l = rb0(x_2);
        uint64_t x_1_r = rb1(x_1 - 1) + 1;
        uint64_t x_2_r = rb1(x_2) + 1;
        return count(x_1_l,x_2_l,y_1,y_2,vs[0],a_l,b_l)+count(x_1_r,x_2_r,y_1,y_2,vs[1],a_r,b_r);
    }
    /// @brief reports the (x, y) coordinates of all the points lying on the
    /// range [x_1 , x_2 ] × [y_1 , y_2 ] of the grid.
    /// @param x_1 
    /// @param x_2 
    /// @param y_1 
    /// @param y_2 
    /// @return 
    vector<Point> report(u32 x_1, u32 x_2, u32 y_1, u32 y_2) {
        vector<Point> p;
        report_a(x_1, x_2, y_1, y_2, wt.root(), 1, symbols.size(), p);
        return p;
    };
    void report_a(u32 x_1, u32 x_2, u32 y_1, u32 y_2, u32 v, u32 a_i, u32 b_i, vector<Point> &p) {
        u32 a = symbols[a_i - 1];
        u32 b = symbols[b_i - 1];

        cout << "symbol range: [ a:" << to_string(a) << ", b:" << to_string(b) << "]" << endl;
        auto seq_vec = wt.seq(v);
        cout << "NodeSeq : ";
        for (auto it = seq_vec.begin(); it!=seq_vec.end(); ++it) {
            u32 value = *it;
            cout << to_string(value) << " ";
        } cout << endl;
        cout << "x_1:" << to_string(x_1) << ", x_2:" << to_string(x_2);
        cout << ", y_1:" << to_string(y_1) << ", y_2:" << to_string(y_2) << endl;
        cout << "-----------" << endl;
        
        if (x_1 > x_2) return;
        if (b < y_1 || y_2 < a) return;
        if (a >= y_1 && b <= y_2) {            
            for (u32 i = x_1; i <= x_2; i++) {
                cout << i << endl;
                p.push_back(Point(outputx(i), outputy(i)));
            }                
        } else {
            auto vs = wt.expand(v); // get the children of this (v) node
            u32 m = (a_i + b_i)/2;
            u32 a_l = a_i;
            u32 b_l = m;
            u32 a_r = m + 1;
            u32 b_r = b_i;             
            uint64_t x_1_l = x_1;
            uint64_t x_2_l = b_l;        
            uint64_t x_1_r = a_r;
            uint64_t x_2_r = x_2;
            report_a(x_1_l, x_2_l, y_1, y_2, vs[0], a_l, b_l, p);
            report_a(x_1_r, x_2_r, y_1, y_2, vs[1], a_r, b_r, p);
        }
    } */
    u32 outputx(u32 x) {
        u32 x_o = wt[x-1];
        return x_o;
    }
    u32 outputy(u32 x) {
        u32 y_o = x;
        return y_o;
    }
};

int no_longer_main(int argc, char* argv[]) {
    string filename = "test.integers";
    if (argc < 2) {
        cout << "Generating test file test.integers" << endl;
        u32 c = 12;
        u32 r = 16;
        vector<Point> points2write = {
            {11,1},{6,2},{6,3},{4,4},{11,4},{2,5},{1,6},{2,8},{10,8},
            {5,10},{12,11},{7,12},{4,13},{11,13},{12,13},{6,15}
        };
        writePointsToFile("test.integers", c, r, points2write);
    } else {
        string filename = argv[1];
    }
    cout << "Reading " << filename << endl;    
    Grid grid(filename);
    printwt(grid.wt);
    cout << endl;
    //cout << "should be 5 ::: " << endl << grid.count(2,11,3,9) << endl;
    //cout << "should be 6 ::: " << endl << grid.count(2,11,3,10) << endl;
    //cout << "should be 3 ::: " << endl << grid.count(1,3,4,9) << endl;

    //vector<Point> p = grid.report(2,11,3,9);
    //printPoints(p);
    return 0;
}