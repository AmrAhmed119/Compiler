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
    std::map<std::string, Symbol*> symbols;
    std::vector<std::string> _grammarLines;
    std::string CFGFilePath;
    void processNonTerminal(std::string& rule);

public:
    ~NonTerminalsCreator() = default;
    explicit NonTerminalsCreator(std::string CFGFilePath);
    void readCFGFile();
    std::set<NonTerminal> getNonTerminals();
    std::vector<std::string> getGrammarLines();
};


#endif //PROJECT_NONTERMINALSCREATOR_H
