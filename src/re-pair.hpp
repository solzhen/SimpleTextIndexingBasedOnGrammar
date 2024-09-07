#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <queue>

std::pair<std::string, int> findMostFrequentPair(const std::string& input);

std::pair<std::string, std::unordered_map<char, std::string>> rePairCompression(std::string input);

std::string decompress(const std::string& compressed, const std::unordered_map<char, std::string>& dictionary);

std::string decompress2(const std::string& compressed, const std::unordered_map<char, std::string>& dictionary);