#include <string>
#include <vector>
#include <queue>
#include <array>
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/bit_vectors.hpp>
#include <algorithm>
#include <utility> // For using std::pair

using namespace sdsl;
using namespace std;

typedef std::pair<int, int> Point;

int count() {
    return 0;
}

vector<Point> report() {
    vector<Point> points;
    return points;
}

bool sortByX(const Point& p1, const Point& p2) {
    return p1.first < p2.first;
}

void writeYToFile(const string& filename, int n_c, int n_r, const vector<Point>& points) {
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

// Write integers into a file
void writeIntegersToFile(const string& filename, int n_c, int n_r, const vector<int>& numbers) {
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

// Read integers from the file into a vector
vector<int> readIntegersFromFile(const string& filename) {
    vector<int> numbers;
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return numbers; // Return an empty vector
    }
    int num;
    while (inputFile.read(reinterpret_cast<char*>(&num), sizeof(num))) {
        numbers.push_back(num);
    }
    inputFile.close();
    return numbers;
}

/// @brief Writes a sequence of integers representing a grid into a file where the first two represent
/// the number of columns and number of rows of the grid, followed by integers where every two integers
/// represent the X (column) and Y (row) coordinates of each point.
/// @param filename filename of file to write
/// @param n_c number of columns of the grid
/// @param n_r number of rows of the grid
/// @param points a vector of std::pair<int int> elements.
void writePointsToFile(const string& filename, int n_c, int n_r, const vector<Point>& points) {
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

/// @brief Reads the integers of a file representing a grid of points, 
/// taking into a account that the first two integers represent the dimensions of the grid
/// and every subsequent pair of integers is a point in the grid as described by its coordinates X,Y
/// @param filename filename of file to read
/// @param columns receives number of columns
/// @param rows receives number of rows
/// @return a std::vector<std::pair<int int>> containing the points
vector<Point> readPointsFromFile(const string& filename, int& columns, int& rows) {
    vector<Point> points;
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return points; // Return an empty vector
    }
    // Read the dimensions of the grid (first two integers in the file)
    inputFile.read(reinterpret_cast<char*>(&columns), sizeof(columns));
    inputFile.read(reinterpret_cast<char*>(&rows), sizeof(rows));

    Point p;
    while (inputFile.read(reinterpret_cast<char*>(&p), sizeof(p))) {
        points.push_back(p);
    }
    inputFile.close();
    return points;
}

/// @brief Print usind std::cout the points in a vector<Point> in the format {x,y}
/// @param points 
void printPoints(vector<Point> points) {
    for (const auto& point : points) {
        cout << "{" << point.first << "," << point.second << "}, ";
    }
    cout << endl;
}

void printIntegers(vector<int> numbers) {
    for (const auto& number : numbers) {
        cout << number << " ";
    }
    cout << endl;
}

void printFile(string filename) {
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    int num;
    while (inputFile.read(reinterpret_cast<char*>(&num), sizeof(num))) {
        cout << num << " ";
    }
}

int readSortAndWriteFromAndToFile(string filename) {
    int c; //columns
    int r; //rows
    vector<Point> points = readPointsFromFile(filename, c, r);
    sort(points.begin(), points.end(), sortByX);
    writePointsToFile(filename, c, r, points);
}


int main(int argc, char* argv[]) {

    if (argc < 2) {
        cout << "Generating test file test.integers" << endl;
        int c = 12;
        int r = 16;
        vector<Point> points2write = {
            {11,1},{6,2},{6,3},{4,4},{11,4},{2,5},{1,6},{2,8},{10,8},
            {5,10},{12,11},{7,12},{4,13},{11,13},{12,13},{6,15}
        };
        writePointsToFile("test.integers", c, r, points2write);
        return 0;
    }

    string filename = argv[1];

    // vector<int> da_points = {
    //     11,1,6,2,6,3,4,4,11,4,2,5,1,6,2,8,10,8,5,10,12,11,7,12,4,13,11,13,12,13,6,15
    // };

    int c; //columns
    int r; //rows
    vector<Point> points = readPointsFromFile(filename, c, r);
    int n = points.size(); // number of points

    bit_vector b(c + n, 0); // mapping bitvector

    cout << "original file " << filename << endl;
    printFile(filename);
    cout << endl;

    // We first sort the pairs by increasing x-
    // coordinate.
    std::sort(points.begin(), points.end(), sortByX);
    writePointsToFile(filename + ".sorted", c, r, points);

    cout << "sorted file " << filename << ".sorted" << endl;
    printFile(filename + ".sorted");
    cout << endl;

    //We then traverse them, writing on B[1, c + n] 
    //a 1 each time the x-coordinate
    //increases and a 0 each time a new 
    //point is found with the current x-coordinate.

    int last_x_coord = 0; // could be 0 as coordinates are 1-indexed
    int i = 0;
    for (const auto& point : points) {        
        if (point.first > last_x_coord) { // x-coord increases
            int d = point.first - last_x_coord; // 1, or if we skipped a few columns we gotta add those 1s
            for (int a=0; a < d; a++) {
                b[i + a] = 1;
            }
            i = i + d;
            last_x_coord = point.first;
        }
        i++;
    }

    //Now we pack the y values in the 
    //first half of the array,

    //writeYToFile(filename + ".sorted.Ypacked", c, r, points );
    writeYToFile(filename + ".sorted.Ypacked", points );

    cout << "sorted and Y packed file " << filename << ".sorted.Ypacked" << endl;
    printFile(filename + ".sorted.Ypacked");
    cout << endl;

    //and use the second half to store the
    //resulting wavelet matrix.

    vector<int> yvalues = readIntegersFromFile(filename + ".sorted.Ypacked");
    //yvalues.erase(yvalues.begin(), yvalues.begin() + 2); //erase column row values
    //cout << yvalues.size() << endl;
    printIntegers(yvalues);

    //string aux = "fehdmjbcolhadmkm";
    wt_blcd_int<> wt;
    construct(wt, filename + ".sorted.Ypacked", 4);

    // for (int i = 0; i < 16; i++) {
    //     size_t count = wt.rank(16, i);
    //     cout << i << ": " << count << endl;
    // }

    // for (int i = 0; i < 16; i++) {
    //     size_t count = wt.select(1,i);
    //     cout << i << ": " << count << endl;
    // }

    //At the end, we free the y i values and build the rank/select structures on B.
    rank_support_v<> rb(&b); // rank structure
    rrr_vector<127> rrrb(b); // rank :: rank_rrrb(i) 0-indexed, excludes i-th position
    rrr_vector<127>::select_1_type select_rrrb(&rrrb); // select :: select_rrrb(i)

    for (int i = 0; i < c + n; i++) {
        cout << b[i] << " ";
    }
    cout << endl;

    string s = "asd";

    
}
