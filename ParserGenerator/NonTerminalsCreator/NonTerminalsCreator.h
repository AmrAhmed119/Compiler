#ifndef PROJECT_NONTERMINALSCREATOR_H
#define PROJECT_NONTERMINALSCREATOR_H


#include <map>
#include <string>
#include <set>
#include <vector>
#include "../Utility/Symbol.h"
#include "../Utility/NonTerminal.h"

class NonTerminalsCreator {
private:
    std::map<std::string, std::shared_ptr<Symbol>> symbols;
    std::vector<std::string> _grammarLines;
    std::string CFGFilePath;
    std::shared_ptr<NonTerminal> createNonTerminalOrGetIfExists(const std::string& name);
    std::shared_ptr<Terminal> createTerminalOrGetIfExists(const std::string& name);
    void processNonTerminal(std::string& rule, bool isStarting = false);
    std::shared_ptr<Production>  processProduction(const std::string& rule);
    std::set<std::shared_ptr<NonTerminal>> getNonTerminals();

public:
    ~NonTerminalsCreator() = default;
    explicit NonTerminalsCreator(std::string CFGFilePath);
    void readCFGFile();
    std::set<std::shared_ptr<NonTerminal>> createNonTerminals();
    std::shared_ptr<NonTerminal> getStartingNonTerminal();
    std::vector<std::string> getGrammarLines();
    static void printNonTerminals(const std::set<std::shared_ptr<NonTerminal>>& nonTerminals);
};


#endif //PROJECT_NONTERMINALSCREATOR_H

