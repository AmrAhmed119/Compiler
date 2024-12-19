#include <iostream>
#include <string>
#include <cstring>
#include <queue>
#include <fstream>
#include <map>
#include <set>
#include <filesystem>
#include "LexicalAnalyzer/RulesParser/RulesParser.h"
#include "LexicalAnalyzer/NFA/NFA.h"
#include "LexicalAnalyzer/DFA/DFA.h"
#include "LexicalAnalyzer/Tokenizer/Tokenizer.h"
#include "LexicalAnalyzer/Utility/Util.h"

const std::string rulesPath = "../LexicalAnalyzer/Inputs/rules.txt";
const std::string intermediatePath = "../LexicalAnalyzer/Inputs/rules.txt_regularExpressions.txt";
const std::string programPath = "../LexicalAnalyzer/Inputs/program.txt";
const std::string tableFilePath = "../LexicalAnalyzer/Outputs/transitionTable.txt";

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

void build() {
    print("STEP1 : Parsing rules...");
    RulesParser rulesParser(rulesPath);
    if (rulesParser.parseFile() == -1) {
        print("Error: Parsing failed", true);
        return;
    }

    print("STEP2 : Starting NFA creation...");
    std::ifstream file(intermediatePath);
    if (!file.is_open()) {
        print("Error: Could not open file.", true);
        return;
    }
    std::set<int> vocab;
    std::map<std::string, std::set<char>> nameToCharSet;
    NFA::processFile(intermediatePath, nameToCharSet, vocab);
    auto startState = NFA::getStartState(file);

    print("STEP3 : Starting DFA creation...");
    DFA *dfa = new DFA();
    dfa->_epsCode = '/l';
    dfa->createDFA(startState, vocab);

    print("STEP4 : Minimizing DFA...");
    std::unordered_set<std::shared_ptr<State>> minimizedDFA = dfa->minimizeDFA();
}

void run() {
    std::cout << "STEP5 : Starting Token extraction..." << std::endl;
//    Tokenizer tokenizer(programPath, tableFilePath);
    performTokenization();

    std::cout << "STEP6 : Starting Parser..." << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 1) {
        print("Error: Invalid number of arguments.", true);
        return 1;
    }

    if (strcmp(argv[1], "--build") == 0) {
        print("Building NFA, DFA, minimized DFA...");
        build();
    } else if (strcmp(argv[1], "--run") == 0) {
        print("Running Tokenizer and Parser...");
        run();
    } else {
        print("Error: Invalid command.", true);
    }
}

