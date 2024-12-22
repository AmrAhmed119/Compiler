#include <stdexcept>
#include "ParserTable.h"


ParserTable::ParserTable(std::map<std::string, NonTerminal> &nonTerminals) : nonTerminals(nonTerminals) {}

void ParserTable::parserTableCreator() {
    for (auto &nonTerminal : nonTerminals) {
        auto &nt = nonTerminal.second;
        for (int i = 0; i < nt.getProductions().size(); i++) {
            auto &production = nt.getProductions()[i];
            auto &firstSet = nt.getFirst()[i];
            for (auto &terminal: firstSet) {
                if (terminal->getIsEpsilon()) {
                    for (auto &terminal: nt.getFollow()) {
                        if(nt.getTransitions().find(terminal) == nt.getTransitions().end()) {
                            nt.addTransition(terminal, production);
                        }
                        else {
                            throw std::runtime_error("Conflict in parsing table");
                        }
                    }
                } else {
                    if(nt.getTransitions().find(terminal) == nt.getTransitions().end()) {
                        nt.addTransition(terminal, production);
                    }
                    else {
                        throw std::runtime_error("Conflict in parsing table");
                    }
                }
            }
        }
    }
}
