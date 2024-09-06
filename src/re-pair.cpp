#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <queue>

// Find the most frequent pair of characters
std::pair<std::string, int> findMostFrequentPair(const std::string& input) {
    std::unordered_map<std::string, int> pairFrequency;

    // Count the frequency of each pair
    for (size_t i = 0; i < input.length() - 1; ++i) {
        std::string pair = input.substr(i, 2);
        pairFrequency[pair]++;
    }

    // Find the pair with the maximum frequency
    std::pair<std::string, int> mostFrequent = { "", 0 };
    for (const auto& entry : pairFrequency) {
        if (entry.second > mostFrequent.second) {
            mostFrequent = entry;
        }
    }

    return mostFrequent;
}

// Re-Pair compression
std::pair<std::string, std::unordered_map<char, std::string>> rePairCompression(std::string input) {
    std::unordered_map<char, std::string> dictionary;
    char newSymbol = 'A'; // Start new symbols from 'A'

    while (true) {
        auto mostFrequentPair = findMostFrequentPair(input);

        // Stop if no frequent pairs are left or newSymbol is out of range
        if (mostFrequentPair.second <= 1 || newSymbol > 'Z') break;

        // Replace all occurrences of the most frequent pair with the new symbol
        std::string pair = mostFrequentPair.first;
        size_t pos;
        while ((pos = input.find(pair)) != std::string::npos) {
            input.replace(pos, 2, std::string(1, newSymbol));
        }

        // Update dictionary with the new symbol
        dictionary[newSymbol] = pair;
        newSymbol++;
    }

    // generate more rules until the encoded string is a single symbol
    char nextSymbol = newSymbol;
    std::string currentEncoded = input;
    while (currentEncoded.length() > 1) {
        std::string pair = currentEncoded.substr(0, 2);
        //replace the first 2 symbols with the next symbol
        currentEncoded.replace(0, 2, std::string(1, nextSymbol));
        dictionary[nextSymbol] = pair;
        nextSymbol++;
    }

    input = currentEncoded;
    return { input, dictionary };
}

// Decompress the string
std::string decompress(const std::string& compressed, const std::unordered_map<char, std::string>& dictionary) {
    std::string output = compressed;

    for (const auto& entry : dictionary) {
        size_t pos;
        while ((pos = output.find(entry.first)) != std::string::npos) {
            output.replace(pos, 1, entry.second);
        }
    }

    return output;
}

/* int main() {

    std::string input;
    std::cout << "Enter the input string: ";
    std::cin >> input;
    auto compressed = rePairCompression(input);
    std::cout << "Compressed: " << compressed.first << std::endl;
    std::cout << "Dictionary of Symbols:" << std::endl;
    for (const auto& entry : compressed.second) {
        std::cout << entry.first << " -> " << entry.second << std::endl;
    }

    std::string decompressed = decompress(compressed.first, compressed.second);
    std::cout << "Decompressed: " << decompressed << std::endl;

    return 0;

} */