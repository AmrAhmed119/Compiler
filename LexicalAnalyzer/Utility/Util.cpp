#include "Util.h"
#include "../Tokenizer/Tokenizer.h"
#include <sstream>
#include <queue>
#include <set>
#include <iostream>
#include <iomanip>

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::pair<std::string, std::string> splitIntoTwo(const std::string& str, char delimiter){
    size_t pos = str.find(delimiter);
    if (pos != std::string::npos){
        std::string before = str.substr(0, pos);
        std::string after = str.substr(pos + 1);
        return make_pair(before, after);
    }
    return make_pair(str, std::string {});
}

void trimBlanksFromEnds(std::string& str){
    const std::string& blanks = " \t\n\r";
    str.erase(str.find_last_not_of(blanks)+1);
    str.erase(0, str.find_first_not_of(blanks));
}

void removeConsecutiveSpaces(std::string& str) {
    std::string result;
    bool seenSpace = false;

    for (char ch : str) {
        if (std::isspace(ch)) {
            if (!seenSpace) {
                result += ' ';
                seenSpace = true;
            }
        } else {
            result += ch;
            seenSpace = false;
        }
    }
    str = result;
}

void removeFirstAndLastChars(std::string& str){
    if (str.size()>2) {
        str.erase(0, 1);
        str.erase(str.length()-1);
    }
}

void printGraph(std::shared_ptr<State> root)
{
    std::queue<std::shared_ptr<State>> s;
    std::set<std::shared_ptr<State>> uq;
    s.push(root);
    uq.insert(root);

    while (!s.empty())
    {
        std::shared_ptr<State> cur = s.front();
        s.pop();
        std::cout << cur << "       ";
        for (auto nextStates : cur->getTransitions())
        {
            std::cout << nextStates.first << "-----> ";
            for (auto state : nextStates.second)
            {
                std::cout << state << "(" << state->getPriority() << ", " << state->isStarting() << "," << state->getTokenClass() << ")" << "\n";
                if (uq.find(state) == uq.end())
                {
                    uq.insert(state);
                    s.push(state);
                }
            }
            std::cout << "        ";
        }
        std::cout << "\n";
    }
}

void performTokenization() {
    Tokenizer tokenizer(programPath, tableFilePath);
    std::ofstream outputFile("../LexicalAnalyzer/Outputs/tokens.txt", std::ios::out);
    if (!outputFile) {
        std::cerr << "Error: Unable to open output file \"tokens\"." << std::endl;
        return;
    }

    outputFile << "Tokenization results for the input program:\n\n";
    while (tokenizer.hasMoreTokens()) {
        std::string token = tokenizer.getNextToken();
        outputFile << token << "\n";
    }

    outputFile << "\n\nTokenization results in detail:\n\n";
    for (const auto &token: tokenizer.getTokens()) {
        outputFile << std::left << std::setw(20) << ("Token: " + token.getValue())
                   << std::setw(20) << ("Class: " + token.getType()) << "\n";
    }

    outputFile << "\n\nSymbol Table:\n\n";
    std::vector<Token> symbolTable = tokenizer.getTokens();
    std::set<std::string> uniqueIdentifiers;
    for (const auto &token: symbolTable) {
        if (token.getType() == "id" && uniqueIdentifiers.find(token.getValue()) == uniqueIdentifiers.end()) {
            uniqueIdentifiers.insert(token.getValue());
            outputFile << std::left << std::setw(40) << ("variable-name: " + token.getValue())
                       << std::setw(20) << ("Class: " + token.getType()) << "\n";
        }
    }

    outputFile.close();
}

void print(const char* message, bool isError) {
    if (isError) {
        std::cerr << message << std::endl;
        return;
    }
    std::cout << message << std::endl;
}
