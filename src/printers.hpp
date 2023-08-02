#include <string>
#include <vector>
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/bit_vectors.hpp>
#include <iostream>
#include <cstdint>
typedef uint32_t u32; // this special trick will save us 1% code width bro trust me
typedef std::pair<u32, u32> Point;
using namespace sdsl;
using namespace std;


/// @brief Print usind std::cout the points in a vector<Point> in the format {x,y}
/// @param points 
void printPoints(vector<Point> points);

void printIntegers(vector<u32> numbers);
template <typename T>
std::string vectorToString(const std::vector<T>& vec, size_t startIndex, size_t endIndex);

template <typename T>
std::string vectorToString(const std::vector<T>& vec);

void printFile(string filename);

void print_vb(const bit_vector &b, u32 n, u32 c) ;

/// @deprecated
void printwtlevel(const wt_blcd_int<> &wt, uint64_t v, size_t level, vector<string>& op, vector<u32> symbols, u32 min, u32 max);
/// @deprecated 
void printwt(const wt_blcd_int<> &wt, vector<u32> symbols);

void printwtlevel(const wt_blcd_int<> &wt, uint64_t v, size_t level, vector<string>& op);

void printwt(const wt_blcd_int<> &wt);