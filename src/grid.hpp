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
#include "wavelet_matrix.hpp"

using namespace sdsl;
using namespace std;

typedef uint32_t u32; // this special trick will save us 1% code width bro trust me
typedef std::pair<u32, u32> Point;

/// @brief A grid representation. Input file is a binary file of a sequence
/// of 4 bytes integers with the format 
/// {columns rows x_1 y_1 x_2 y_2 ... x_n y_n}
class Grid {
private:
    string filename; // external file   
    u32 c; // number of columns
    u32 r; // number of rows
    u32 n; // nubmer of points
    vector<Point> points; // Grid points
    WaveletMatrix wt; // Wavelet tree
    PreprocessedBitvector bv;
    u32 count(u32 x_1, u32 x_2, u32 y_1, u32 y_2, u32 l, u32 a, u32 b);
    vector<Point> report(u32 x_1, u32 x_2, u32 y_1, u32 y_2, u32 l, u32 a, u32 b);
    u32 outputx(u32 level, u32 x);
    u32 outputy(u32 level, u32 a, u32 b, u32 i);
public:
    // Constructor
    Grid(const string& fn);
    u32 count(u32 x_1, u32 x_2, u32 y_1, u32 y_2);
    vector<Point> report(u32 x_1, u32 x_2, u32 y_1, u32 y_2);
    void printself();
};