#include <utility>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string>
#include <stack>
#include <queue>
#include <set>
#include "../../LexicalAnalyzer/Tokenizer/Tokenizer.h"

#ifndef SYNTAX_DIRECTED_TRANSLATOR_TOPDOWNPARSER_H
#define SYNTAX_DIRECTED_TRANSLATOR_TOPDOWNPARSER_H

using namespace std;

class TopDownParser {
public:
    TopDownParser(std::map<std::string, NonTerminal> nonTerminalMap, 
                  std::vector<std::string> nonTerminals, 
                  Tokenizer tokenizer);

    std::vector<std::string> parse();

private:
    // Data members
    std::map<std::string, NonTerminal> nonTerminalMap; // Map of non-terminals
    std::vector<std::string> nonTerminals;             // List of non-terminals
    std::queue<std::string> input;                     // Input tokens
    std::stack<std::string> stk;                       // Parsing stack

    // Helper functions
    bool isTerminal(std::string token); // Checks if a token is a terminal
    std::stack<std::string> addProductionRuleToStack(std::stack<std::string> tokenStack, const std::vector<std::string>& productionRule);
    std::string addStackToLevel(std::stack<std::string> tokenStack, std::string levelStr);
};

#endif // SYNTAX_DIRECTED_TRANSLATOR_TOPDOWNPARSER_H
