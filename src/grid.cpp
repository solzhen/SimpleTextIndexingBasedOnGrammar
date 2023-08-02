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

using namespace sdsl;
using namespace std;

bool DEBUG_MODE = 0;

typedef uint32_t u32; // this special trick will save us 1% code width bro trust me
typedef std::pair<u32, u32> Point;

bool sortByX(const Point& p1, const Point& p2) {
    return p1.first < p2.first;
}


/// @brief Print usind std::cout the points in a vector<Point> in the format {x,y}
/// @param points 
void printPoints(vector<Point> points) {
    for (const auto& point : points) {
        cout << "{" << point.first << "," << point.second << "},";
    }
}

void printIntegers(vector<u32> numbers) {
    for (const auto& number : numbers) {
        cout << number << " ";
    }
}

template <typename T>
std::string vectorToString(const std::vector<T>& vec, size_t startIndex, size_t endIndex) {
    if (startIndex >= vec.size() || endIndex > vec.size() || startIndex > endIndex) {
        return "Invalid indices or empty range.";
    }

    std::stringstream ss;
    ss << "[";
    for (size_t i = startIndex; i < endIndex; ++i) {
        ss << vec[i];
        if (i < endIndex - 1) {
            ss << ",";
        }
    }
    ss << "]";
    return ss.str();
}

template <typename T>
std::string vectorToString(const std::vector<T>& vec) {
    return vectorToString(vec, 0, vec.size());
}

void printFile(string filename) {
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    u32 num;
    while (inputFile.read(reinterpret_cast<char*>(&num), sizeof(num))) {
        cout << num << " ";
    }
}

void print_vb(const bit_vector &b, u32 n, u32 c) {
    for (u32 i = 0; i < c + n; i++) {
        cout << b[i] << " ";
    }
}

/// @deprecated
void printwtlevel(const wt_blcd_int<> &wt, uint64_t v, size_t level, vector<string>& op, vector<u32> symbols, u32 min, u32 max) {
    if (op.size() < level+1) {
        op.push_back(""); // + 1 level
    }
    op[level] += vectorToString(symbols, min, max) + "{";
    auto seq_vec = wt.seq(v);
    for (auto it = seq_vec.begin(); it!=seq_vec.end(); ++it) {
        u32 value = *it;
        op[level] += to_string(value);
        op[level] += ",";
    }
    op[level].pop_back();
    op[level] += "}   ";
    if (!wt.is_leaf(v)) {
        auto vs = wt.expand(v);
        printwtlevel(wt, vs[0], level + 1, op, symbols, min, (min+max+1)/2);
        printwtlevel(wt, vs[1], level + 1, op, symbols, (min+max+1)/2, max);
    }
}
/// @deprecated 
void printwt(const wt_blcd_int<> &wt, vector<u32> symbols) {
    vector<string> op(2,""); // at least two levels
    printwtlevel(wt, wt.root(), 0, op, symbols, 0, symbols.size());
    for (string lvl : op) {        
        cout << lvl << endl;
    }
}


void printwtlevel(const wt_blcd_int<> &wt, uint64_t v, size_t level, vector<string>& op) {
    if (op.size() < level+1) {
        op.push_back(""); // + 1 level
    }
    op[level] += "{";
    auto seq_vec = wt.seq(v);
    for (auto it = seq_vec.begin(); it!=seq_vec.end(); ++it) {
        u32 value = *it;
        op[level] += to_string(value);
        op[level] += ",";
    }
    op[level].pop_back();
    op[level] += "}  ";
    if (!wt.is_leaf(v)) {
        auto vs = wt.expand(v);
        printwtlevel(wt, vs[0], level + 1, op);
        printwtlevel(wt, vs[1], level + 1, op);
    }
}

void printwt(const wt_blcd_int<> &wt) {
    vector<string> op(2,""); // at least two levels
    printwtlevel(wt, wt.root(), 0, op);
    for (string lvl : op) {        
        cout << lvl << endl;
    }
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
                for (u32 a=0; a < d; a++) {
                    b[i + a] = 1;
                }
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
    /// @brief returns the number of points lying on the rectangle
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
        if (DEBUG_MODE) {
            cout << "symbol range: [" << to_string(a) << "," << to_string(b) << "]" << endl;
            auto seq_vec = wt.seq(v);
            cout << "NodeSeq : ";
            for (auto it = seq_vec.begin(); it!=seq_vec.end(); ++it) {
                u32 value = *it;
                cout << to_string(value) << " ";
            } cout << endl;
            cout << "x_1:" << to_string(x_1) << ", x_2:" << to_string(x_2) << endl;
        }
        if (x_1 > x_2) return 0;
        if (b < y_1 || y_2 < a) return 0;
        if (a >= y_1 && b <= y_2) return x_2 - x_1 + 1;
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
        if (x_1 > x_2) return;
        if (b < y_1 || y_2 < a) return;
        if (a >= y_1 && b <= y_2) {
            for (u32 i = x_1; i <= x_2; i++)
                p.push_back(Point(outputx(i), outputy(i)));
        }
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
    u32 outputx(u32 i) {
        return 0;
    }
    u32 outputy(u32 i) {
        return 0;
    }
};

int main(int argc, char* argv[]) {
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
    //printwt(grid.wt);
    //printIntegers(grid.symbols);
    //cout << endl;

    cout << "should be 5 ::: " << endl << grid.count(2,11,3,9) << endl;
    cout << "should be 6 ::: " << endl << grid.count(2,11,3,10) << endl;
    cout << "should be 3 ::: " << endl << grid.count(1,3,4,9) << endl;

    return 0;
}