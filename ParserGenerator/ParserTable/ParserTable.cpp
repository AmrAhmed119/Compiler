#include <stdexcept>
#include <fstream>
#include <iostream>
#include "ParserTable.h"


ParserTable::ParserTable(std::map<std::string, std::shared_ptr<NonTerminal>> &nonTerminals) : nonTerminals(nonTerminals) {}


void ParserTable::parserTableCreator() {
    for (auto &nonTerminal : nonTerminals) {
        auto &nt = nonTerminal.second;
        bool hasEpsilon = false;
        for (int i = 0; i < nt->getProductions().size(); i++) {
            auto &production = nt->getProductions()[i];
            auto &firstSet = nt->getFirst()[i];
            for (auto &terminal: firstSet) {
                if (terminal->getIsEpsilon()) {
                    hasEpsilon = true;
                    for (auto &terminal: nt->getFollow()) {
                        if(nt->getTransitions().find(terminal) == nt->getTransitions().end()) {
                            nt->addTransition(terminal, production);
                        }
                        else {
                            throw std::runtime_error("Conflict in parsing table");
                        }
                    }
                } else {
                    if(nt->getTransitions().find(terminal) == nt->getTransitions().end()) {
                        nt->addTransition(terminal, production);
                    }
                    else {
                        throw std::runtime_error("Conflict in parsing table");
                    }
                }
            }
        }
        if (!hasEpsilon) {
            for (auto &terminal: nt->getFollow()) {
                if(nt->getTransitions().find(terminal) == nt->getTransitions().end()) {
                    // create production with sync
                    Production syncProduction = Production();
                    syncProduction.addSymbol(std::make_shared<Symbol>("sync", true));
                    nt->addTransition(terminal, std::make_shared<Production>(syncProduction));
                }
            }
        }
    }
}

void ParserTable::printTable(std::string filePath) {
    std::ofstream file(filePath);
    for (auto &nonTerminal : nonTerminals) {
        auto &nt = nonTerminal.second;
        file << "NonTerminal: " << nt->getName() << std::endl;
        for (auto &transition : nt->getTransitions()) {
            file << "Terminal: " << transition.first->getName() << " Production: ";
            for (auto &symbol : transition.second->getSymbols()) {
                file << symbol->getName() << " ";
            }
            file << std::endl;
        }
    }
    file.close();
}