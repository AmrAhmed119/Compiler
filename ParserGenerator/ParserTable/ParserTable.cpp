#include <stdexcept>
#include "ParserTable.h"


ParserTable::ParserTable(std::map<std::string, NonTerminal> &nonTerminals) : nonTerminals(nonTerminals) {}


void ParserTable::parserTableCreator() {
    for (auto &nonTerminal : nonTerminals) {
        auto &nt = nonTerminal.second;
        bool hasEpsilon = false;
        for (int i = 0; i < nt.getProductions().size(); i++) {
            auto &production = nt.getProductions()[i];
            auto &firstSet = nt.getFirst()[i];
            for (auto &terminal: firstSet) {
                if (terminal->getIsEpsilon()) {
                    hasEpsilon = true;
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
        if (!hasEpsilon) {
            for (auto &terminal: nt.getFollow()) {
                if(nt.getTransitions().find(terminal) == nt.getTransitions().end()) {
                    // create production with sync
                    Production syncProduction = Production();
                    syncProduction.addSymbol(std::make_shared<Symbol>("sync", true));
                    nt.addTransition(terminal, std::make_shared<Production>(syncProduction));
                }
                else {
                    throw std::runtime_error("Conflict in parsing table sync case");
                }
            }
        }
    }
}
