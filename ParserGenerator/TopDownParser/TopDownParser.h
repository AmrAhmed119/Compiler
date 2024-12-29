#ifndef TOPDOWNPARSER_H
#define TOPDOWNPARSER_H

#include "../../LexicalAnalyzer/Tokenizer/Tokenizer.h"
#include "../Utility/Symbol.h"
#include "../Utility/Terminal.h"
#include "../Utility/Production.h"
#include <map>
#include <memory>
#include <stack>
#include <queue>
#include <string>
#include <vector>
#include <fstream>

class TopDownParser {
public:
    TopDownParser(
        std::map<std::string, std::shared_ptr<Symbol>> nonTerminalMap,
        std::vector<std::string> nonTerminals, 
        Tokenizer& tokenizer
    );

    // Main parsing function
    std::vector<std::string> parse();

    // Parse and output results to a file
    void parseToFile(const std::string& outputFilename);
    void print(
        const std::vector<std::string>& output
    );

    void printSententialForm(const std::vector<std::string> &sententialForm);

private:
    // Member variables
    std::map<std::string, std::shared_ptr<Symbol>> nonTerminalMap;
    std::vector<std::string> nonTerminals;
    std::queue<std::string> input;
    std::stack<std::string> stk;

    // Helper functions
    bool isTerminal(const std::string& token);
    std::stack<std::string> addProductionRuleToStack(
        std::stack<std::string> tokenStack, 
        const std::vector<std::shared_ptr<Symbol>>& productionRule
    );
    bool recoverUsingSync(const std::string& nonTerminal);
    void updateLeftmostDerivation(
        std::vector<std::string>& output, 
        const std::string& nonTerminal, 
        const std::vector<std::shared_ptr<Symbol>>& production
    );
    void writeSententialForm(
        std::ofstream& outputFile, 
        const std::vector<std::string>& output
    );
    
};

#endif // TOPDOWNPARSER_H
