#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <fstream>
#include <cstdint>

typedef uint16_t u16;
typedef std::pair<u16, u16> Pair;

bool print = false;

// Re-Pair compression
// we should obtain an encoded string and a dictionary of rules
// the dictionary is in the form rule : left_side_symbol + right_side_symbol
// if a symbol is less than 256 then it's a terminal symbol, else it's a rule index

struct PairHash {
    std::size_t operator()(const Pair& p) const {
        return std::hash<u16>()(p.first) ^ std::hash<u16>()(p.second);
    }
};

// Find the most frequent pair of characters
std::pair<Pair, int> findMostFrequentPair(const std::vector<u16>& input) {    
    
    std::unordered_map<Pair, u16, PairHash> pairFrequency;

    // Count the frequency of each pair
    for (size_t i = 0; i < input.size() - 1; ++i) {
        Pair pair = Pair(input[i], input[i + 1]); // input[i](i, 2);
        pairFrequency[pair]++;
    }

    // Find the pair with the maximum frequency
    std::pair<Pair, int> mostFrequent = { Pair(0,0), 0 };
    for (const auto& entry : pairFrequency) {
        if (entry.second > mostFrequent.second) {
            mostFrequent = entry;
        }
    }

    return mostFrequent;
}

// Re-Pair compression
// returns a pair where the first member is a vector of symbols, 
// and the second a map of rules where each symbol is mapped to a pair of symbols
// in the form A -> B C.
std::pair<std::vector<u16>, std::unordered_map<u16, Pair>> rePairCompression(std::vector<u16> input) {
    std::unordered_map<u16, Pair> dictionary; // rules
    u16 newSymbol = (u16)256; // Start new symbols from 256

    while (true) {
        auto mostFrequentPair = findMostFrequentPair(input);

        if (print) {
            u16 mfs1 = mostFrequentPair.first.first;
            u16 mfs2 = mostFrequentPair.first.second;
            if (mfs1 > 'z') mfs1 += 'A'-256;
            if (mfs2 > 'z') mfs2 += 'A'-256;
            std::cout << "MFS: " << (char)mfs1 << " " << (char)mfs2 << "   f: " << mostFrequentPair.second << std::endl;
        }        

        // Stop if no frequent pairs are left, that is there are not repeated pairs
        if (mostFrequentPair.second <= 1) break;

        // Replace all occurrences of the most frequent pair with the new symbol
        Pair pair = mostFrequentPair.first;
        for (size_t i = 0; i < input.size() - 1; ++i) {
            if (input[i] == pair.first && input[i + 1] == pair.second) {
                input[i] = newSymbol;
                input.erase(input.begin() + i + 1);
            }
        }
        // Update dictionary with the new symbol
        dictionary[newSymbol] = pair;
        newSymbol++;

        // print input
        if (print) {
            std::cout << "Input: ";
            for (u16 symbol : input) {
                if (symbol > 'z') symbol += 'A'-256;
                std::cout << (char) symbol << " ";
            }
            std::cout << std::endl;
        }     
    }

    if (print) {
        std::cout << "Input: ";
        for (u16 symbol : input) {
            if (symbol > 'z') symbol += 'A'-256;
            std::cout << (char) symbol << " ";
        }
        std::cout << std::endl;
    }

    newSymbol++;

    // now that there are no repeating pairs, keep replacing every 2 elements until there's only 1 symbol left
    while (input.size() > 1) {
        for (int i = 0; i < input.size() - 1; i += 2) {
            dictionary[newSymbol] = Pair(input[i], input[i + 1]);
            input[i] = newSymbol;
            input.erase(input.begin() + i + 1);
            newSymbol++;
        }
        if (print) {
            std::cout << "Input: ";
            for (u16 symbol : input) {
                if (symbol > 'z') symbol += 'A'-256;
                std::cout << (char) symbol << " ";
            }
            std::cout << std::endl;
        }        
    }
    
    return { input, dictionary };
}


// Re-Pair decompression
std::vector<u16> decompress(const std::vector<u16> compressed, const std::unordered_map<u16, Pair>& dictionary) {
    std::vector<u16> decompressed;
    for (u16 symbol : compressed) {
        if (dictionary.find(symbol) != dictionary.end()) { // symbol is a rule
            Pair decompressedSymbol = dictionary.at(symbol);
            u16 leftSide = decompressedSymbol.first;
            u16 rightSide = decompressedSymbol.second;

            std::vector<u16> leftSideDecompressed = decompress({ leftSide }, dictionary);
            std::vector<u16> rightSideDecompressed = decompress({ rightSide }, dictionary);

            // Concatenate the decompressed left and right sides
            decompressed.insert(decompressed.end(), leftSideDecompressed.begin(), leftSideDecompressed.end());
            decompressed.insert(decompressed.end(), rightSideDecompressed.begin(), rightSideDecompressed.end());
        }
        else { // symbol is a terminal
            decompressed.push_back(symbol);
        }
    }
    return decompressed;
}

/* 
int main( int argc, char* argv[] ) {

    // set print 
    print = false;
    if (argc > 1) {
        print = true;
    }

    std::string input;
    std::cout << "Enter the input string: ";
    std::cin >> input;
    // convert string input into vector<u16>
    std::vector<u16> inputVector;
    for (char c : input) {
        inputVector.push_back((u16)c);
    }
    auto compressed = rePairCompression(inputVector);
    std::string output;
    // convert compressed back into string
    for (u16 symbol : compressed.first) {
        std::cout << symbol << " ";
        std::cout << std::endl;
        output += symbol -256+'A';
    }
    std::cout << "Compressed: " << output << std::endl;

    std::cout << "Dictionary of Symbols:" << std::endl;
    for (const auto& entry : compressed.second) {
        u16 left = entry.second.first;
        u16 right = entry.second.second;
        if (left > 255) left += 'A'-256;
        if (right > 255) right += 'A'-256;
        std::cout << (char)(entry.first-256+'A') << " -> " << (char)left << ", " << (char)right << std::endl;
    }

    std::vector<u16> decompressed = decompress(compressed.first, compressed.second);
    // convert decompressed back into string
    std::string decompressedString;
    for (u16 symbol : decompressed) {
        decompressedString += symbol;
    }
    std::cout << "Decompressed: " << decompressedString << std::endl;

    return 0;

} */