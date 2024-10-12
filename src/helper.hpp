#include <string>
#include <iostream>
#include <cstdint>
int isTerminal(char c);
bool isPrefix(const std::string& str, const std::string& prefix);
std::vector<uint8_t> generateRandomChars(int n);