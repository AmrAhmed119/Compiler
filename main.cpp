#include <iostream>
#include <string>
#include "LexicalAnalyzer/RulesParser/RulesParser.h"


int main() {
    std::string filePath = "/media/hussein/Data/data/Github Repos/Compiler/LexicalAnalyzer/Inputs/rules.txt";
    RulesParser rulesParser(filePath);
    int statusCode = rulesParser.parseFile();
    if (statusCode == -1)
    {
        std::cerr << "Error: Parsing failed" << std::endl;
        return -1;
    }
    return 0;
}
