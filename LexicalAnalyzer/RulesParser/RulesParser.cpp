
#include <fstream>
#include <iostream>
#include <algorithm>
#include "RulesParser.h"
#include "../Utility/Util.h"

RulesParser::RulesParser(std::string filePath) {
    this->filePath = filePath;
    this->linesCounter = 0;
}

int RulesParser::parseFile() {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: File not found" << std::endl;
        return -1;
    }

    std::string line;
    while (std::getline(file, line)) {
        linesCounter++;
        trimBlanksFromEnds(line);
        int type = checkType(line);
        if (type == -1) {
            std::cerr << "Error: Invalid line " << linesCounter << std::endl;
            return -1;
        }

        if(type == KEYWORDS)
            keywordsHandler(line);
        else if(type == PUNCTUATION)
            punctuationHandler(line);
        else if(type == REGULAR_DEFINITION){
            if (regularDefinitionHandler(line) == -1) {
                std::cerr << "Error: Invalid regular definition at line " << linesCounter << std::endl;
                return -1;
            }
        }
        else if(type == REGULAR_EXPRESSION){
            if (regularExpressionHandler(line) == -1) {
                std::cerr << "Error: Invalid regular expression at line " << linesCounter << std::endl;
                return -1;
            }
        }

    }
    file.close();

    // Convert all keywords to regular expressions with the highest priority
    for (const std::string& keyword: allKeywords){
        RegularExpression regularExpression(keyword, keyword, 0);
        regularExpressions.push_back(regularExpression);
    }

    // Convert all punctuation symbols to regular expressions with the second-highest priority
    for (const std::string& punctuation: allPunctuation){
        RegularExpression regularExpression(punctuation, punctuation, 1);
        regularExpressions.push_back(regularExpression);
    }

    std::sort(regularExpressions.begin(), regularExpressions.end(),
              [](RegularExpression &a, RegularExpression &b) {
        return a.getPriority() < b.getPriority();
    });

    // write regular expressions to file
    std::ofstream regularExpressionsFile(filePath+"_regularExpressions.txt");
    for (const RegularExpression& regularExpression: regularExpressions)
        regularExpressionsFile << regularExpression.getName() << std::endl<< regularExpression.getRegex() << std::endl;
    regularExpressionsFile.close();
    return 0;
}

int RulesParser::checkType(std::string str) {
    if (str.front() == '{' && str.back() == '}')
        return KEYWORDS;
    if (str.front() == '[' && str.back() == ']')
        return PUNCTUATION;
    if (isRegularDefinition(str))
        return REGULAR_DEFINITION;
    if (isRegularExpression(str))
        return REGULAR_EXPRESSION;

    return -1;
}

bool RulesParser::isRegularDefinition(std::string str) {
     size_t pos = str.find('=');
     if (pos != std::string::npos) {
        char before = str.at(pos - 1); // character before '='
        if (before != '\\')
            return true;
    }
    return false;
}

bool RulesParser::isRegularExpression(std::string str) {
    size_t pos = str.find(':');
    if (pos != std::string::npos) {
        char before = str.at(pos - 1); // character before ':'
        if (before != '\\')
            return true;
    }
    return false;
}

void RulesParser::keywordsHandler(std::string str) {
    removeFirstAndLastChars(str);
    trimBlanksFromEnds(str);
    removeConsecutiveSpaces(str);
    std::vector<std::string> keywords = split(str, ' ');
    for (const std::string& keyword: keywords)
        allKeywords.push_back(keyword);
}

void RulesParser::punctuationHandler(std::string str) {
    removeFirstAndLastChars(str);
    trimBlanksFromEnds(str);
    removeConsecutiveSpaces(str);
    std::vector<std::string> punctuations = split(str, ' ');
    for (const std::string& punctuation: punctuations)
        allPunctuation.push_back(punctuation);
}

int RulesParser::regularDefinitionHandler(std::string str) {
    removeConsecutiveSpaces(str);
    std::pair<std::string, std::string> splitString = splitIntoTwo(str, '=');
    trimBlanksFromEnds(splitString.first);
    trimBlanksFromEnds(splitString.second);
    RegularDefinition regularDefinition(splitString.first, splitString.second);
    int statusCode = regularDefinition.standardizeRegex(regularDefinitions);
    if (statusCode == -1)
        return -1;
    regularDefinitions.push_back(regularDefinition);
    return 0;
}

int RulesParser::regularExpressionHandler(std::string str) {
    removeConsecutiveSpaces(str);
    std::pair<std::string, std::string> splitString = splitIntoTwo(str, ':');
    trimBlanksFromEnds(splitString.first);
    trimBlanksFromEnds(splitString.second);
    RegularExpression regularExpression(splitString.first, splitString.second, (int) regularExpressions.size() + 2);
    int statusCode = regularExpression.standardizeRegex(regularDefinitions);
    if (statusCode == -1)
        return -1;
    regularExpressions.push_back(regularExpression);
    return 0;
}


