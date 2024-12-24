#ifndef PROJECT_LL1GRAMMARCONVERTER_H
#define PROJECT_LL1GRAMMARCONVERTER_H


#include "../Utility/NonTerminal.h"

class LL1GrammarConverter {
private:
    std::vector<std::shared_ptr<NonTerminal>> nonTerminals;
    void eliminateLeftRecursion();
    void performLeftFactoring();
    void factorizeNonTerminal(const std::shared_ptr<NonTerminal>& nonTerminal);
    void solveImmediateLeftRecursion(std::shared_ptr<NonTerminal>& nonTerminal);
    static void solveNonImmediateLeftRecursion(std::shared_ptr<NonTerminal>& nonTerminalA, std::shared_ptr<NonTerminal>& nonTerminalB);

public:
    explicit LL1GrammarConverter(std::set<std::shared_ptr<NonTerminal>>& nonTerminalSet);
    std::set<std::shared_ptr<NonTerminal>> convertToLL1();
};


#endif //PROJECT_LL1GRAMMARCONVERTER_H
