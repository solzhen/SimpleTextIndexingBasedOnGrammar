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
#include "wavelet_matrix.hpp"

using namespace sdsl;
using namespace std;

typedef uint32_t u32; // this special trick will save us 1% code width bro trust me
typedef std::pair<u32, u32> Point;

/// @brief A grid representation.  
/// {columns rows x_1 y_1 x_2 y_2 ... x_n y_n}
class Grid {
public:
    //string filename; // external file   
    u32 c; // number of columns
    u32 r; // number of rows
    u32 n; // nubmer of points
    //vector<Point> points; // Grid points
    WaveletMatrix wt; // Wavelet tree
    //PreprocessedBitvector bv;

    u32 count(u32 x_1, u32 x_2, u32 y_1, u32 y_2, u32 l, u32 a, u32 b);
    vector<Point> report(u32 x_1, u32 x_2, u32 y_1, u32 y_2, u32 l, u32 a, u32 b);
    u32 outputx(u32 level, u32 x);
    u32 outputy(u32 level, u32 a, u32 b, u32 i);
    Grid();
    /// @brief Construct a grid from a binary file
    /// @param fn Filename of the binary file
    /// @note The binary file should contain the dimensions of the grid first 
    /// (columns, rows), followed by the points as pairs of integers. Every integer
    /// in the file should be a 4-byte long unsigned integer (uint32_t).
    /// The coordinates should be 0-indexed.
    Grid(const string& fn);
    /// @brief Construct a grid from a vector of points
    /// The points must be 1-indexed
    /// @param points A vector of points
    /// @param columns Number of columns
    /// @param rows Number of rows
    Grid(std::vector<Point> points, u32 columns, u32 rows);
    /// @brief Construct a grid, for use exclusively with pattern search
    /// @param points A vector of n points, pre-sorted by X, 1-indexed, with unique X values ranging from 1 to n
    Grid(std::vector<Point> points);
    /// @brief Count the number of points in the grid that are within the rectangle
    /// @param x_1 1-indexed column range start
    /// @param x_2 1-indexed column range end
    /// @param y_1 1-indexed row range start
    /// @param y_2 1-indexed row range end
    /// @return The number of points in the grid that are
    /// within the rectangle as an integer
    u32 count(u32 x_1, u32 x_2, u32 y_1, u32 y_2);
    /// @brief Report the points in the grid that are within the rectangle
    /// @param x_1 1-indexed column range start
    /// @param x_2 1-indexed column range end
    /// @param y_1 1-indexed row range start
    /// @param y_2 1-indexed row range end
    /// @return A vector of points that are within the rectangle
    vector<Point> report(u32 x_1, u32 x_2, u32 y_1, u32 y_2);
    void printself();
    u32 getColumns() { return c; }
    u32 getRows() { return r; }
    WaveletMatrix getWaveletMatrix() { return wt; }
    /// @brief Access the number in the i-th 1-indexed position
    /// @param i 
    /// @return 
    u32 access(u32 i) {return wt.access(i-1);};
    /// @brief Returns the 1-indexed position of the j-th occurrence of c
    /// @param j 
    /// @param c 
    /// @return 
    u32 select(u32 j, u32 c) {return wt.select(j, c);};
    long long bitsize() { return wt.bitsize() + 3 * 32; }
};