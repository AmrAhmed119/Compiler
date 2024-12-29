#include "../TopDownParser/TopDownParser.h"
#include "../Utility/NonTerminal.h"
#include <map>
#include <memory>
#include <algorithm>
#include <fstream>
#include <vector>
#include <stack>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "TopDownParser.h"

// Constructor
TopDownParser::TopDownParser(
    std::map<std::string, std::shared_ptr<Symbol>> nonTerminalMap,
    std::vector<std::string> nonTerminals,
    Tokenizer &tokenizer)
    : nonTerminalMap(std::move(nonTerminalMap)),
      nonTerminals(std::move(nonTerminals)),
      input(), stk()
{
    while (tokenizer.hasMoreTokens())
    {
        input.push(tokenizer.getNextToken());
    }
    input.push("$");
    auto start = this->nonTerminals[0];
    for (const auto &[_, symbol] : this->nonTerminalMap)
    {
        if (auto nt = std::dynamic_pointer_cast<NonTerminal>(symbol); nt)
        {
            start = nt->getIsStarting() ? nt->getName() : start;
        }
    }
    stk.push("$");
    stk.push(start); // Push the starting non-terminal
}

void TopDownParser::printStack()
{
    std::ofstream outFile("../ParserGenerator/Outputs/parser_output.txt", std::ios::app);
    if (!outFile.is_open())
    {
        std::cerr << "Error: Could not open parser_output.txt for writing." << std::endl;
        return;
    }

    std::stack<std::string> temp = stk;
    outFile << "Stack: ";
    while (!temp.empty())
    {
        outFile << temp.top() << " ";
        temp.pop();
    }
    outFile << "\n";

    outFile.close();
}

void printMatch(std::string str, bool isMatched)
{
    std::ofstream outFile("../ParserGenerator/Outputs/parser_output.txt", std::ios::app);
    if (!outFile.is_open())
    {
        std::cerr << "Error: Could not open parser_output.txt for writing." << std::endl;
        return;
    }
    std::string matchStatus = isMatched ? "Matched " : "Miss Matched ";
    outFile << matchStatus << " String: ";
    outFile << str;
    outFile << "\n";

    outFile.close();
}

std::vector<std::string> TopDownParser::parse()
{
    std::vector<std::string> parseOutput;
    std::vector<std::string> sententialForm;

    // Initial step (root of the grammar)

    sententialForm.push_back(nonTerminals[0]);
    parseOutput.push_back(nonTerminals[0]);

    // Main parsing loop (expansion of the grammar)
    while (!input.empty() && !stk.empty())
    {
        std::string top = stk.top();
        printStack();
        stk.pop();
        std::string currentInput = input.front();

        if (isTerminal(top))
        {
            if (top == "\\L")
                continue;
            if (top == currentInput)
            {
                sententialForm.push_back(top);
                parseOutput.push_back("Match Terminal: " + top); // Add terminal symbol
                printMatch(top, true);
                input.pop();
            }
            else
            {
                parseOutput.push_back("Mismatch: " + top + " vs " + currentInput);
                printMatch(top, false);
                input.pop();
            }
        }
        else
        {

            auto nonTerminalIt = nonTerminalMap.find(top);
            if (nonTerminalIt == nonTerminalMap.end())
            {
                std::cerr << "Non-terminal not found: " << top << std::endl;
                continue;
            }

            std::shared_ptr<NonTerminal> nonTerminalObj = std::dynamic_pointer_cast<NonTerminal>(nonTerminalIt->second);
            const auto &transitions = nonTerminalObj->getTransitions();

            auto ter = std::dynamic_pointer_cast<Terminal>(this->nonTerminalMap[currentInput]);
            auto productionIt = transitions.find(ter);

            if (productionIt == transitions.end())
            {
                std::cerr << "No production rule: " << top << " with input " << currentInput << std::endl;
                continue;
            }

            const std::shared_ptr<Production> &production = productionIt->second;
            stk = addProductionRuleToStack(stk, production->getSymbols());

            // Build output for the current derivation step
            std::string derivationStep;
            for (const auto &symbol : production->getSymbols())
            {
                if (symbol->getName() != "\\L")
                {
                    sententialForm.push_back(symbol->getName());
                    derivationStep += symbol->getName() + " "; // Accumulate symbols in one line
                }
            }

            // Remove extra space at the end of the derivation step
            if (!derivationStep.empty())
            {
                derivationStep.pop_back(); // Remove the trailing space
                parseOutput.push_back(derivationStep);
            }

            // Print the current derivation step (for debugging)
            // print(parseOutput);
        }
        // printSententialForm(sententialForm);
    }

    return parseOutput;
}

void TopDownParser::printSententialForm(const std::vector<std::string> &sententialForm)
{
    std::ofstream outFile("parser_output.txt", std::ios::app);
    if (!outFile.is_open())
    {
        std::cerr << "Error: Could not open parser_output.txt for writing." << std::endl;
        return;
    }

    // Add each sentential form to the output file
    std::vector<std::string> currentForm = sententialForm;
    for (const auto &symbol : currentForm)
    {
        outFile << symbol << " ";
    }
    outFile << "\n"; // Ensure we add a new line after each sentential form

    outFile.close();
}

void TopDownParser::print(const std::vector<std::string> &output)
{
    std::ofstream outFile("parser_output.txt", std::ios::app);
    if (!outFile.is_open())
    {
        std::cerr << "Error: Could not open parser_output.txt for writing." << std::endl;
        return;
    }

    // Print the current output in the exact format requested
    for (const auto &symbol : output)
    {
        outFile << symbol << " ";
    }
    outFile << "\n"; // New line after each derivation step

    outFile.close();
}

// Parse and output to a file
void TopDownParser::parseToFile(const std::string &outputFilename)
{
    // Open the output file
    std::ofstream outputFile(outputFilename);

    if (!outputFile.is_open())
    {
        std::cerr << "Error: Could not open file " << outputFilename << " for writing." << std::endl;
        return;
    }

    std::vector<std::string> output; // Holds the leftmost derivation

    while (!input.empty() && !stk.empty())
    {
        std::string top = stk.top();
        stk.pop();
        std::string currentInput = input.front();

        if (isTerminal(top))
        {
            if (top == "\\L")
            {
                continue; // Ignore epsilon transitions
            }
            // Terminal handling
            if (top == currentInput)
            {
                output.push_back(top);
                input.pop(); // Match
            }
            else
            {
                // Terminal mismatch: skip input (error recovery)
                outputFile << "Error: Expected " << top << " but found " << currentInput << ". Skipping input.\n";
                input.pop();
            }
        }
        else
        {
            // Non-terminal handling
            auto nonTerminalIt = nonTerminalMap.find(top);
            if (nonTerminalIt == nonTerminalMap.end())
            {
                outputFile << "Error: Non-terminal " << top << " not found in nonTerminal map. Skipping.\n";
                continue;
            }

            std::shared_ptr<NonTerminal> nonTerminalObj = std::dynamic_pointer_cast<NonTerminal>(nonTerminalIt->second);

            const auto &transitions = nonTerminalObj->getTransitions();
            auto ter = std::dynamic_pointer_cast<Terminal>(this->nonTerminalMap[currentInput]);
            auto productionIt = transitions.find(ter);

            if (productionIt == transitions.end())
            {
                outputFile << "Error: No production rule for " << top << " with input " << currentInput << ".\n";

                bool recovered = recoverUsingSync(top);
                if (!recovered)
                {
                    outputFile << "Error: Could not recover from input mismatch.\n";
                    input.pop(); // Skip current input
                }
                continue;
            }

            const std::shared_ptr<Production> &production = productionIt->second;
            stk = addProductionRuleToStack(stk, production->getSymbols());

            // Update the output (leftmost derivation)
            updateLeftmostDerivation(output, top, production->getSymbols());
        }

        // Write the current derivation to the file
        writeSententialForm(outputFile, output);
    }

    // Final validation
    if (!input.empty())
    {
        outputFile << "Error: Remaining input after parsing: " << input.front() << "\n";
    }
    if (!stk.empty())
    {
        outputFile << "Error: Stack not empty after parsing: " << stk.top() << "\n";
    }

    outputFile.close(); // Close the file
}

// Utility functions
bool TopDownParser::isTerminal(const std::string &token)
{
    return (std::find(nonTerminals.begin(), nonTerminals.end(), token) == nonTerminals.end()) | (token == "$");
}

std::stack<std::string> TopDownParser::addProductionRuleToStack(
    std::stack<std::string> tokenStack,
    const std::vector<std::shared_ptr<Symbol>> &productionRule)
{
    for (auto it = productionRule.rbegin(); it != productionRule.rend(); ++it)
    {
        tokenStack.push((*it)->getName());
    }
    return tokenStack;
}

bool TopDownParser::recoverUsingSync(const std::string &nonTerminal)
{
    while (!input.empty())
    {
        std::string currentInput = input.front();

        for (const auto &[symbolName, nonTerminalObj] : nonTerminalMap)
        {
            if (symbolName == nonTerminal)
            {
                for (const auto &transition : std::dynamic_pointer_cast<NonTerminal>(nonTerminalObj)->getTransitions())
                {
                    if (transition.first->getName() == currentInput && transition.first->getIsSync())
                    {
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

void TopDownParser::updateLeftmostDerivation(
    std::vector<std::string> &output,
    const std::string &nonTerminal,
    const std::vector<std::shared_ptr<Symbol>> &production)
{
    auto it = std::find(output.begin(), output.end(), nonTerminal);
    if (it != output.end())
    {
        output.erase(it); // Remove the non-terminal
        for (auto prodIt = production.rbegin(); prodIt != production.rend(); ++prodIt)
        {
            output.insert(it, (*prodIt)->getName()); // Insert the production symbols
        }
    }
}

void TopDownParser::writeSententialForm(std::ofstream &outputFile, const std::vector<std::string> &output)
{
    for (const auto &symbol : output)
    {
        outputFile << symbol << " ";
    }
    outputFile << "\n"; // New line after the sentential form
}
