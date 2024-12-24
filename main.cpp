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
//    std::cout << "STEP5 : Starting Token extraction..." << std::endl;
//    Tokenizer tokenizer(programPath, tableFilePath);
//    performTokenization();

//    std::cout << "STEP6 : Starting Creation of non terminals from CFG file..." << std::endl;
//    NonTerminalsCreator nonTerminalsCreator(CFGFilePath);
//    nonTerminalsCreator.readCFGFile();
//    std::vector<std::string> grammarLines = nonTerminalsCreator.getGrammarLines();
//
//    print("STEP7 : Printing nonTerminals...");
//    std::set<std::shared_ptr<NonTerminal>> nonTerminals = nonTerminalsCreator.createNonTerminals();
//    NonTerminalsCreator::printNonTerminals(nonTerminals);
    print("STEP8 : Testing Left Recursion Elimination...");
    std::set<std::shared_ptr<NonTerminal>> nonTerminals;

    // create some rules here

    // create non terminals named A, B
    auto A = std::make_shared<NonTerminal>("A");
    auto B = std::make_shared<NonTerminal>("B");

    // create terminals named a,c,d
    auto a = std::make_shared<Terminal>("a");
    auto c = std::make_shared<Terminal>("c");
    auto d = std::make_shared<Terminal>("d");

    // create a production A -> Ba
    std::vector<std::shared_ptr<Symbol>> symbols;
    symbols.push_back(B);
    symbols.push_back(a);
    auto production = std::make_shared<Production>(symbols);
    A->addProduction(production);

    // create a production B -> Ac | d
    std::vector<std::shared_ptr<Symbol>> symbols2;
    symbols2.push_back(A);
    symbols2.push_back(c);
    auto production2 = std::make_shared<Production>(symbols2);
    B->addProduction(production2);

    std::vector<std::shared_ptr<Symbol>> symbols3;
    symbols3.push_back(d);
    auto production3 = std::make_shared<Production>(symbols3);
    B->addProduction(production3);

    nonTerminals.insert(A);
    nonTerminals.insert(B);


    LL1GrammarConverter ll1GrammarConverter(nonTerminals);
    auto newNonTerminals = ll1GrammarConverter.convertToLL1();
    NonTerminalsCreator::printNonTerminals(newNonTerminals);

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
