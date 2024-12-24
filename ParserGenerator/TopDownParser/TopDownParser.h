#ifndef PROJECT_TOPDOWNPARSER_H
#define PROJECT_TOPDOWNPARSER_H

#include "../Utility/NonTerminal.h"
#include "../../LexicalAnalyzer/Tokenizer/Tokenizer.h"
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <memory>
#include <queue>

class TopDownParser {
private:
    // Map of NonTerminals with their respective data
    std::map<std::string, NonTerminal> nonTerminalMap;

    // List of non-terminal strings
    std::vector<std::string> nonTerminals;

    // Input queue to store tokens
    std::queue<std::string> input;

    // Stack for parsing process
    std::stack<std::string> stk;

    /**
     * Helper function to determine if a token is a terminal.
     * @param token The token to check.
     * @return True if the token is a terminal, false otherwise.
     */
    bool isTerminal(const std::string& token);

    /**
     * Helper function to add production rule symbols to the stack.
     * @param tokenStack The stack to which symbols are added.
     * @param productionRule The symbols of the production rule.
     * @return The updated stack.
     */
    std::stack<std::string> addProductionRuleToStack(
        std::stack<std::string> tokenStack,
        const std::vector<std::shared_ptr<Symbol>>& productionRule);

    bool recoverUsingSync(const std::string &nonTerminal);

    void print(const std::string &top, const std::string &currentInput, std::vector<std::string> &output);

    /**
     * Helper function to add the contents of a stack to a level string.
     * @param tokenStack The stack to process.
     * @param levelStr The string to append the stack contents to.
     * @return The updated string.
     */
    std::string addStackToLevel(std::stack<std::string> tokenStack, std::string levelStr);

public:
    /**
     * Constructor for the TopDownParser.
     * @param nonTerminalMap A map of string to NonTerminal objects.
     * @param nonTerminals A list of non-terminal symbols.
     * @param tokenizer The tokenizer to generate input tokens.
     */
    TopDownParser(
        std::map<std::string, NonTerminal> nonTerminalMap,
        std::vector<std::string> nonTerminals,
        Tokenizer tokenizer);

    /**
     * Parses the input tokens and produces an output sequence.
     * @return A vector of parsed strings, or an empty vector on failure.
     */
    std::vector<std::string> parse();
};

#endif // PROJECT_TOPDOWNPARSER_H
