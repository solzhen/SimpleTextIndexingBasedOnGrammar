#include <string>
#include <vector>
#include <iostream>
#include <cstdint>
#include <utility>
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/bit_vectors.hpp>

using namespace sdsl;
using namespace std;

typedef std::pair<uint32_t, uint32_t> Point;

/// @brief Write the second coordinates of the points in a vector 
/// representing a grid into a file but only after writing the dimensions of the grid
/// @param fn Filename
/// @param nc number of columns
/// @param nr number of rows
/// @param ps vector of Points
void writeYToFile(const string& fn, uint32_t nc, uint32_t nr, const vector<Point>& ps);

/// @brief Write the second coordinates of the points in a vector 
/// representing a grid into a file
/// @param fn Filename
/// @param ps vector of Points
void writeYToFile(const string& fn, const vector<Point>& ps);

/// @brief Writes a vector of integers into a file following nc and nr (dimension of a grid)
/// @param fn Filename
/// @param nc number of columns
/// @param nr number of rows
/// @param ns vector of uint32 numbers
void writeIntegersToFile(const string& fn, uint32_t nc, uint32_t nr, const vector<uint32_t>& ns);

/// @brief Read integers from the file into a vector
/// @param fn Filename
/// @return A vector of unsigned 32 bit integers
vector<uint32_t> readIntegersFromFile(const string& fn);

/// @brief Writes a sequence of integers representing a grid into a file where the first two represent
/// the number of columns and number of rows of the grid, followed by integers where every two integers
/// represent the X (column) and Y (row) coordinates of each point.
/// @param fn filename of file to write
/// @param nc number of columns of the grid
/// @param nr number of rows of the grid
/// @param ps a vector of std::pair<int int> elements.
void writePointsToFile(const string& fn, uint32_t nc, uint32_t nr, const vector<Point>& ps);

/// @brief Reads the integers of a file representing a grid of points, 
/// taking into a account that the first two integers represent the dimensions of the grid
/// and every subsequent pair of integers is a point in the grid as described by its coordinates X,Y
/// @param fn filename of file to read
/// @param cs receives number of columns
/// @param rs receives number of rows
/// @return a std::vector<std::pair<int int>> containing the points
vector<Point> readPointsFromFile(const string& fn, uint32_t& cs, uint32_t& rs);