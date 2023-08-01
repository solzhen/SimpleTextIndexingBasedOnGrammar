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


typedef std::pair<uint32_t, uint32_t> Point;

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

void printIntegers(vector<uint32_t> numbers) {
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
    uint32_t num;
    while (inputFile.read(reinterpret_cast<char*>(&num), sizeof(num))) {
        cout << num << " ";
    }
}

void print_vb(const bit_vector &b, uint32_t n, uint32_t c) {
    for (uint32_t i = 0; i < c + n; i++) {
        cout << b[i] << " ";
    }
}

/// @deprecated
void printwtlevel(const wt_blcd_int<> &wt, uint64_t v, size_t level, vector<string>& op, vector<uint32_t> symbols, uint32_t min, uint32_t max) {
    if (op.size() < level+1) {
        op.push_back(""); // + 1 level
    }
    op[level] += vectorToString(symbols, min, max) + "{";
    auto seq_vec = wt.seq(v);
    for (auto it = seq_vec.begin(); it!=seq_vec.end(); ++it) {
        uint32_t value = *it;
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
void printwt(const wt_blcd_int<> &wt, vector<uint32_t> symbols) {
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
        uint32_t value = *it;
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
    uint32_t c; // number of columns
    uint32_t r; // number of rows
    uint32_t n; // nubmer of points
    vector<Point> points; // Grid points
    wt_blcd_int<> wt; // Wavelet tree
    bit_vector b; // mapping bitvector
    rrr_vector<127> rrrb;
    rrr_vector<127>::rank_1_type rank_1;
    rrr_vector<127>::rank_0_type rank_0;
    rrr_vector<127>::select_1_type select_1;
    rrr_vector<127>::select_0_type select_0;
    // Constructor
    Grid(const string& fn) : filename(fn) {
        points = readPointsFromFile(filename, c, r); // symbol range is = [1, rows]
        n = points.size();
        b = bit_vector(n + c);
        // We first sort the pairs by x-coord
        std::sort(points.begin(), points.end(), sortByX);
        writePointsToFile(filename, c, r, points);
        // build map bitvector
        uint32_t last_x_coord = 0;
        int i = 0;
        for (const auto& point : points) {        
            if (point.first > last_x_coord) { // x-coord increases
                uint32_t d = point.first - last_x_coord; // 1, or if we skipped a few columns we gotta add those 1s
                for (uint32_t a=0; a < d; a++) {
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
    int count(uint32_t x_1, uint32_t x_2, uint32_t y_1, uint32_t y_2) {
        return count(x_1, x_2, y_1, y_2, 1, 1, this->r);
    };
    /// @brief Range [x_1, x_2] of level l, inside a segment of S_l that represents
    /// symbols in [a, b] and symbol range [y_1 , y_2].
    /// @return The number of symbols in S_l [x_1 , x_2] belonging to [y_1 , y_2].
    int count(uint32_t x_1, uint32_t x_2, uint32_t y_1, uint32_t y_2, int l, uint32_t a, uint32_t b) {
        if (x_1 > x_2) return 0;
        if (b < y_1 || y_2 < a) return 0;
        if (a >= y_1 && b <= y_2) return x_2 - x_1 + 1;
        uint32_t x_1_l = rank_0(x_1 - 1) + 1; // get the mapped x_1 value
        uint32_t x_2_l = rank_0(x_2);   // same with x_2
        /*[x_1,x_2] × [y_1,y_2] is mapped to 
        [select_1(x_1) − x_1 + 1, select_1(x_2 + 1) − (x_2 + 1)]*/
        return 0;
    }
    /// @brief reports the (x, y) coordinates of all the points lying on the
    /// range [x_1 , x_2 ] × [y_1 , y_2 ] of the grid.
    /// @param x_1 
    /// @param x_2 
    /// @param y_1 
    /// @param y_2 
    /// @return 
    vector<Point> report(uint32_t x_1, uint32_t x_2, uint32_t y_1, uint32_t y_2) {
        vector<Point> p;
        return p;
    };
};

int main(int argc, char* argv[]) {
    string filename = "test.integers";
    if (argc < 2) {
        cout << "Generating test file test.integers" << endl;
        uint32_t c = 12;
        uint32_t r = 16;
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
    return 0;
}