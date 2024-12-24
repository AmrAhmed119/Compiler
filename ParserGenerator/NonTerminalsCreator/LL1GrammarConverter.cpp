//
// Created by Hp on 12/24/2024.
//

#include "LL1GrammarConverter.h"

const std::string EPSILON = "\\L";

std::set<std::shared_ptr<NonTerminal>> convertToSet(std::vector<std::shared_ptr<NonTerminal>> &nonTerminals);

std::set<std::shared_ptr<NonTerminal>> convertToSet(std::vector<std::shared_ptr<NonTerminal>> &nonTerminals) {
    std::set<std::shared_ptr<NonTerminal>> nonTerminalSet;
    for (const auto &nonTerminal : nonTerminals) {
        nonTerminalSet.insert(nonTerminal);
    }
    return nonTerminalSet;
}

LL1GrammarConverter::LL1GrammarConverter(std::set<std::shared_ptr<NonTerminal>> &nonTerminalSet) {
    for (const auto &nonTerminal : nonTerminalSet) {
        nonTerminals.push_back(nonTerminal);
    }
}

std::set<std::shared_ptr<NonTerminal>> LL1GrammarConverter::convertToLL1() {
    eliminateLeftRecursion();
    performLeftFactoring();
    return convertToSet(nonTerminals);
}

void LL1GrammarConverter::eliminateLeftRecursion() {
    int size = (int)nonTerminals.size();
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < i; j++) {
            solveNonImmediateLeftRecursion(nonTerminals[i], nonTerminals[j]);
        }
        solveImmediateLeftRecursion(nonTerminals[i]);
    }
}

void LL1GrammarConverter::performLeftFactoring() {

}

void LL1GrammarConverter::solveImmediateLeftRecursion(std::shared_ptr<NonTerminal> &nonTerminal) {
    std::vector<std::vector<std::shared_ptr<Symbol>>> alphas, betas;

    // get alphas and betas
    for (const auto &production : nonTerminal->getProductions()) {
        if (production->getFirstSymbolName() == nonTerminal->getName()) {
            std::vector<std::shared_ptr<Symbol>> newSymbols;
            auto symbols = production->getSymbols();
            for (int i = 1; i < symbols.size(); i++) {
                newSymbols.push_back(symbols[i]);
            }
            alphas.push_back(newSymbols);
        } else {
            std::vector<std::shared_ptr<Symbol>> newSymbols;
            for (const auto &symbol : production->getSymbols()) {
                newSymbols.push_back(symbol);
            }
            betas.push_back(newSymbols);
        }
    }

    if (alphas.empty()) return;

    // create new non-terminal
    std::shared_ptr<NonTerminal> newNonTerminal = std::make_shared<NonTerminal>(nonTerminal->getName() + "'");

    // create new productions
    std::vector<std::shared_ptr<Production>> newProductions;

    for (const auto &beta : betas) {
        std::shared_ptr<Production> newProduction = std::make_shared<Production>();
        for (const auto &symbol : beta) {
            if (symbol->getName() != EPSILON) {
                newProduction->addSymbol(symbol);
            }
        }
        newProduction->addSymbol(newNonTerminal);
        newProductions.push_back(newProduction);
    }

    if (betas.empty()) {
        std::shared_ptr<Production> newProduction = std::make_shared<Production>();
        newProduction->addSymbol(newNonTerminal);
        newProductions.push_back(newProduction);
    }

    nonTerminal->setProductions(newProductions);

    // create new productions for new non-terminal
    newProductions.clear();
    for (const auto &alpha : alphas) {
        std::shared_ptr<Production> newProduction = std::make_shared<Production>();
        for (const auto &symbol : alpha) {
            if (symbol->getName() != EPSILON) {
                newProduction->addSymbol(symbol);
            }
        }
        newProduction->addSymbol(newNonTerminal);
        newProductions.push_back(newProduction);
    }
    // add epsilon production
    std::shared_ptr<Production> epsilonProduction = std::make_shared<Production>();
    epsilonProduction->addSymbol(std::make_shared<Terminal>(EPSILON));
    newProductions.push_back(epsilonProduction);

    newNonTerminal->setProductions(newProductions);
    nonTerminals.push_back(newNonTerminal);
}

void LL1GrammarConverter::solveNonImmediateLeftRecursion(std::shared_ptr<NonTerminal> &nonTerminalI, std::shared_ptr<NonTerminal> &nonTerminalJ) {
    std::vector<std::shared_ptr<Production>> newProductions;
    auto productionsJ = nonTerminalJ->getProductions();

    for (const auto &productionI : nonTerminalI->getProductions()) {
        if (productionI->getFirstSymbolName() == nonTerminalJ->getName()) {
            auto symbolsI = productionI->getSymbols();
            for (const auto &productionJ : productionsJ) {
                std::shared_ptr<Production> newProduction = std::make_shared<Production>();
                auto symbolsJ = productionJ->getSymbols();
                // append all symbols of productionJ
                for (const auto &symbol : symbolsJ) {
                    if (symbol->getName() != EPSILON) {
                        newProduction->addSymbol(symbol);
                    }
                }
                // append all symbols of productionI except the first symbol
                for (int i = 1; i < symbolsI.size(); i++) {
                    newProduction->addSymbol(symbolsI[i]);
                }
                if (newProduction->getSymbols().empty()) {
                    newProduction->addSymbol(std::make_shared<Terminal>(EPSILON));
                }
                newProductions.push_back(newProduction);
            }
        } else {
            newProductions.push_back(productionI);
        }
    }

    nonTerminalI->setProductions(newProductions);
}
