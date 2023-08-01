#include <vector>
#include <iostream>
#include <cstdint>
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/bit_vectors.hpp>

using namespace sdsl;
using namespace std;

#include "file_handler.hpp"

void writeYToFile(const string& filename, uint32_t n_c, uint32_t n_r, const vector<Point>& points) {
    std::ofstream outputFile(filename, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    Point p = {n_c, n_r};
    outputFile.write(reinterpret_cast<const char*>(&p), sizeof(p));
    for (const Point& p : points) {
        outputFile.write(reinterpret_cast<const char*>(&p.second), sizeof(p.second));
    }
    outputFile.close();
}

void writeYToFile(const string& filename, const vector<Point>& points) {
    std::ofstream outputFile(filename, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    for (const Point& p : points) {
        outputFile.write(reinterpret_cast<const char*>(&p.second), sizeof(p.second));
    }
    outputFile.close();
}

void writeIntegersToFile(const string& filename, uint32_t n_c, uint32_t n_r, const vector<uint32_t>& numbers) {
    std::ofstream outputFile(filename, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    for (const int num : numbers) {
        outputFile.write(reinterpret_cast<const char*>(&num), sizeof(num));
    }
    outputFile.close();
}

vector<uint32_t> readIntegersFromFile(const string& filename) {
    vector<uint32_t> numbers;
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return numbers; // Return an empty vector
    }
    uint32_t num;
    while (inputFile.read(reinterpret_cast<char*>(&num), sizeof(num))) {
        numbers.push_back(num);
    }
    inputFile.close();
    return numbers;
}

void writePointsToFile(const string& filename, uint32_t n_c, uint32_t n_r, const vector<Point>& points) {
    std::ofstream outputFile(filename, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    Point p = {n_c, n_r};
    outputFile.write(reinterpret_cast<const char*>(&p), sizeof(p));
    for (const Point& p : points) {
        outputFile.write(reinterpret_cast<const char*>(&p), sizeof(p));
    }
    outputFile.close();
}

vector<Point> readPointsFromFile(const string& filename, uint32_t& columns, uint32_t& rows) {
    vector<Point> points;
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return points; // Return an empty vector
    }
    inputFile.read(reinterpret_cast<char*>(&columns), sizeof(columns));
    inputFile.read(reinterpret_cast<char*>(&rows), sizeof(rows));
    Point p;
    while (inputFile.read(reinterpret_cast<char*>(&p), sizeof(p))) {
        points.push_back(p);
    }
    inputFile.close();
    return points;
}
