//
// Created by hussein on 11/29/24.
//

#ifndef COMPILER_RULESPARSER_H
#define COMPILER_RULESPARSER_H


#include <string>
#include <vector>
#include "RegularExpression.h"
#include "RegularDefinition.h"


class RulesParser{
private:
    int linesCounter;
    std::string filePath;
    std::vector<RegularExpression> regularExpressions;
    std::vector<RegularDefinition> regularDefinitions;
    std::vector<std::string> allKeywords;
    std::vector<std::string> allPunctuation;

    static bool isRegularDefinition(std::string str);
    static bool isRegularExpression(std::string str);
    void keywordsHandler(std::string str);
    void punctuationHandler(std::string str);
    int regularDefinitionHandler(std::string str);
    int regularExpressionHandler(std::string str);

public:
    static constexpr int KEYWORDS = 0;
    static constexpr int PUNCTUATION = 1;
    static constexpr int REGULAR_DEFINITION = 2;
    static constexpr int REGULAR_EXPRESSION = 3;
    RulesParser(std::string filePath);
    static int checkType(std::string str);
    int parseFile();

};
#endif //COMPILER_RULESPARSER_H
