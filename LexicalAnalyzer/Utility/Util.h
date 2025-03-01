
#ifndef COMPILER_UTIL_H
#define COMPILER_UTIL_H

#include <vector>
#include <string>
#include "State.h"

const int epsilon = 23628;
const std::string rulesPath = "../LexicalAnalyzer/Inputs/rules.txt";
const std::string intermediatePath = "../LexicalAnalyzer/Inputs/rules.txt_regularExpressions.txt";
const std::string programPath = "../LexicalAnalyzer/Inputs/program.txt";
const std::string tableFilePath = "../LexicalAnalyzer/Outputs/transitionTable.txt";

std::vector<std::string> split(const std::string& str, char delimiter);
std::vector<std::string> splitWithStringDelimiter(const std::string& str, const std::string& delimiter);
std::pair<std::string, std::string> splitIntoTwo(const std::string& str, char delimiter);

void trimBlanksFromEnds(std::string& str);
void removeConsecutiveSpaces(std::string& str);
void removeSpaces(std::string& str);
void removeFirstAndLastChars(std::string& str);
void printGraph(std::shared_ptr<State> root);
void print(const char* message, bool isError = false);
void performTokenization();


#endif //COMPILER_UTIL_H
