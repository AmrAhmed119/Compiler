#include "TopDownParser.h"
#include "../Utility/NonTerminal.h"
#include <map>

TopDownParser::TopDownParser(map<string, NonTerminal> nonTerminalMap, vector<string> nonTerminals, Tokenizer tokenizer)
    : nonTerminalMap(std::move(nonTerminalMap)), nonTerminals(std::move(nonTerminals)), input(), stk() {
    while (tokenizer.hasMoreTokens()) {
        input.push(tokenizer.getNextToken());
    }
}

vector<string> TopDownParser::parse() {
    vector<string> output;

    while (!input.empty() && !stk.empty()) {
        string top = stk.top();
        stk.pop();
        string currentInput = input.front();

        if (isTerminal(top)) {
            if (top == currentInput) {
                output.push_back(top);
                input.pop();
            } else {
                cerr << "Error: Expected " << top << " but found " << currentInput << endl;
                return {};
            }
        } else {
            // Look up the NonTerminal in the map
            auto nonTerminalIt = nonTerminalMap.find(top);
            if (nonTerminalIt == nonTerminalMap.end()) {
                cerr << "Error: Non-terminal " << top << " not found in nonTerminal map." << endl;
                return {};
            }

            NonTerminal& nonTerminalObj = nonTerminalIt->second;

            // Use the transitions map to find the production for the current input
            auto productionIt = nonTerminalObj.transitions.find(currentInput);
            if (productionIt == nonTerminalObj.transitions.end()) {
                cerr << "Error: No production rule for " << top << " with input " << currentInput << endl;
                return {};
            }

            const Production& production = productionIt->second;

            // Push the production rule's RHS onto the stack
            stk = addProductionRuleToStack(stk, production.rhs);
        }
    }

    if (!input.empty() || !stk.empty()) {
        cerr << "Error: Parsing failed. Remaining input or unmatched stack." << endl;
        return {};
    }

    return output;
}

bool TopDownParser::isTerminal(string token) {
    return find(nonTerminals.begin(), nonTerminals.end(), token) == nonTerminals.end();
}

string TopDownParser::addStackToLevel(stack<string> tokenStack, string levelStr) {
    while (!tokenStack.empty()) {
        levelStr += tokenStack.top() + " ";
        tokenStack.pop();
    }
    return levelStr;
}

stack<string> TopDownParser::addProductionRuleToStack(stack<string> tokenStack, const vector<string>& productionRule) {
    for (auto it = productionRule.rbegin(); it != productionRule.rend(); ++it) {
        tokenStack.push(*it);
    }
    return tokenStack;
}
