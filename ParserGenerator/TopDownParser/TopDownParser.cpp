#include "TopDownParser.h"
#include "../Utility/NonTerminal.h"
#include <map>
#include <memory>
#include <algorithm>

TopDownParser::TopDownParser(
        std::map<std::string, std::shared_ptr<NonTerminal>> nonTerminalMap,
    std::vector<std::string> nonTerminals, 
    Tokenizer& tokenizer)
    : nonTerminalMap(std::move(nonTerminalMap)), 
      nonTerminals(std::move(nonTerminals)), 
      input(), stk() {
    while (tokenizer.hasMoreTokens()) {
        input.push(tokenizer.getNextToken());
    }
    stk.push(this->nonTerminals[0]); // Push the starting non-terminal
}

std::vector<std::string> TopDownParser::parse() {
    std::vector<std::string> output;

    while (!input.empty() && !stk.empty()) {
        std::string top = stk.top();
        stk.pop();
        std::string currentInput = input.front();

        if (isTerminal(top)) {
            // Terminal handling
            if (top == currentInput) {
                output.push_back(top);
                input.pop(); // Match
            } else {
                // Terminal mismatch: skip input (error recovery)
                std::cerr << "Error: Expected " << top << " but found " << currentInput << ". Skipping input." << std::endl;
                input.pop();
            }
        } else {
            // Non-terminal handling
            auto nonTerminalIt = nonTerminalMap.find(top);
            if (nonTerminalIt == nonTerminalMap.end()) {
                // Non-terminal not found
                std::cerr << "Error: Non-terminal " << top << " not found in nonTerminal map. Skipping." << std::endl;
                continue; // Skip and move on
            }

            std::shared_ptr<NonTerminal> nonTerminalObj = nonTerminalIt->second;

            // Find production rule
            const auto& transitions = nonTerminalObj->getTransitions();
            auto productionIt = transitions.find(std::make_shared<Terminal>(currentInput, false));
            if (productionIt == transitions.end()) {
                // No valid production rule for this input
                std::cerr << "Error: No production rule for " << top << " with input " << currentInput << std::endl;

                // Error recovery using isSync
                bool recovered = recoverUsingSync(top);
                if (!recovered) {
                    std::cerr << "Error: Could not recover from input mismatch." << std::endl;
                    input.pop(); // Skip current input
                }
                continue;
            }

            const std::shared_ptr<Production>& production = productionIt->second;

            // Push the production rule's RHS onto the stack
            stk = addProductionRuleToStack(stk, production->getSymbols());
        }

        // Print the current state
        print(top, currentInput, output);
    }

    // Final validation
    if (!input.empty()) {
        std::cerr << "Error: Remaining input after parsing: " << input.front() << std::endl;
    }
    if (!stk.empty()) {
        std::cerr << "Error: Stack not empty after parsing: " << stk.top() << std::endl;
    }

    return output;
}

bool TopDownParser::isTerminal(const std::string& token) {
    return std::find(nonTerminals.begin(), nonTerminals.end(), token) == nonTerminals.end();
}

std::string TopDownParser::addStackToLevel(std::stack<std::string> tokenStack, std::string levelStr) {
    while (!tokenStack.empty()) {
        levelStr += tokenStack.top() + " ";
        tokenStack.pop();
    }
    return levelStr;
}

std::stack<std::string> TopDownParser::addProductionRuleToStack(
    std::stack<std::string> tokenStack, 
    const std::vector<std::shared_ptr<Symbol>>& productionRule) {
    for (auto it = productionRule.rbegin(); it != productionRule.rend(); ++it) {
        tokenStack.push((*it)->getName());
    }
    return tokenStack;
}

bool TopDownParser::recoverUsingSync(const std::string& nonTerminal) {
    while (!input.empty()) {
        std::string currentInput = input.front();

        for (const auto& [symbolName, nonTerminalObj] : nonTerminalMap) {
            if (symbolName == nonTerminal) {
                for (const auto& transition : nonTerminalObj->getTransitions()) {
                    if (transition.first->getName() == currentInput && transition.first->getIsSync()) {
                        std::cerr << "Recovered at sync symbol: " << currentInput << std::endl;
                        return true;
                    }
                }
            }
        }

        // Skip input until a sync symbol is found
        input.pop();
    }

    return false;
}

void TopDownParser::print(const std::string& top, const std::string& currentInput, std::vector<std::string>& output) {
    // Collect the current stack state
    std::string levelStr = addStackToLevel(stk, "Current Level: "); 

    std::cout << levelStr << std::endl;
    std::cout << "Parsing: " << top << " with input: " << currentInput << std::endl;

    std::cout << "Current Output: ";
    for (const auto& symbol : output) {
        std::cout << symbol << " ";
    }
    std::cout << std::endl;
}
