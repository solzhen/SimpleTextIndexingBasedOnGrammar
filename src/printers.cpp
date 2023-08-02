#include "printers.hpp"

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