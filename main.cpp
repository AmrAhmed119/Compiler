#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <map>
#include <set>
#include <filesystem>
#include "LexicalAnalyzer/RulesParser/RulesParser.h"
#include "LexicalAnalyzer/NFA/NFA.h"
#include "LexicalAnalyzer/DFA/DFA.h"
#include "LexicalAnalyzer/Tokenizer/Tokenizer.h"
#include "LexicalAnalyzer/Utility/Util.h"
#include "ParserGenerator/NonTerminalsCreator/NonTerminalsCreator.h"
#include "ParserGenerator/Utility/Util.h"

#include <map>
#include <string>
#include "ParserGenerator/Utility/Terminal.h"
#include "ParserGenerator/Utility/NonTerminal.h"
#include "ParserGenerator/Utility/Production.h"
#include <vector>
#include <iostream>
#include "ParserGenerator/NonTerminalsCreator/NonTerminalsCreator.h"
#include "ParserGenerator/NonTerminalsCreator/LL1GrammarConverter.h"
#include "ParserGenerator/ParserTable/ParserTable.h"
#include "ParserGenerator/TopDownParser/TopDownParser.h"

void build()
{
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
    dfa->_epsCode = epsilon;
    dfa->createDFA(startState, vocab);

    print("STEP4 : Minimizing DFA...");
    std::unordered_set<std::shared_ptr<State>> minimizedDFA = dfa->minimizeDFA();
}

void run() {
    std::cout << "STEP5 : Starting Token extraction..." << std::endl;
    Tokenizer tokenizer(programPath, tableFilePath);

    std::cout << "STEP6 : Starting Creation of non terminals from CFG file..." << std::endl;
    NonTerminalsCreator nonTerminalsCreator(CFGFilePath);
    nonTerminalsCreator.readCFGFile();

    std::vector<std::string> grammarLines = nonTerminalsCreator.getGrammarLines();

    std::set<std::shared_ptr<NonTerminal>> nonTerminals = nonTerminalsCreator.createNonTerminals();
    print("STEP7 : Printing nonTerminals...");
    NonTerminalsCreator::printNonTerminals(nonTerminals);

    std::map<std::string, std::shared_ptr<NonTerminal>> hussienMap;
    for (const auto& nonTerminal : nonTerminals) {
        hussienMap[nonTerminal->getName()] = nonTerminal;
    }

    ParserTable parserTable(hussienMap);
    parserTable.parserTableCreator();
    parserTable.printTable(parseTableFilePath);

    print("STEP8 : Parsing table created successfully.");

    std::vector<std::string> nonTerminalsNames;
    for (const auto& nonTerminal : nonTerminals) {
        nonTerminalsNames.push_back(nonTerminal->getName());
    }



// Run the parser and get the derivation steps
TopDownParser topDownParser(nonTerminalsCreator.symbols, nonTerminalsNames, tokenizer);
std::vector<std::string> parseOutput = topDownParser.parse();

// Write the output to a file
// const std::string outputFilename = "../ParserGenerator/Outputs/parser_output.txt";
// std::ofstream outputFile(outputFilename);
// if (outputFile.is_open()) {
//     for (const auto& line : parseOutput) {
//         outputFile << line << std::endl;
//     }
//     outputFile.close();
//     std::cout << "Parsing complete. Output written to " << outputFilename << std::endl;
// } else {
//     std::cerr << "Error: Unable to open file " << outputFilename << " for writing." << std::endl;
// }

print("STEP9 : Parsing completed successfully.");


}

int main(int argc, char* argv[]) {
    // if (argc < 1) {
    //     print("Error: Invalid number of arguments.", true);
    //     return 1;
    // }

    // if (strcmp(argv[1], "--build") == 0) {
    //     print("Building NFA, DFA, minimized DFA...");
    //     build();
    // } else if (strcmp(argv[1], "--run") == 0) {
    //     print("Running Tokenizer and Parser...");
    //     run();
    // } else {
    //     print("Error: Invalid command.", true);
    // }
    run();
}
