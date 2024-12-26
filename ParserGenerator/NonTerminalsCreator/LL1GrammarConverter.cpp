#include <iostream>
#include "LL1GrammarConverter.h"

const std::string EPSILON = "\\L";

std::set<std::shared_ptr<NonTerminal>> convertToSet(std::vector<std::shared_ptr<NonTerminal>> &nonTerminals);

std::set<std::shared_ptr<NonTerminal>> convertToSet(std::vector<std::shared_ptr<NonTerminal>> &nonTerminals)
{
    std::set<std::shared_ptr<NonTerminal>> nonTerminalSet;
    for (const auto &nonTerminal : nonTerminals)
    {
        nonTerminalSet.insert(nonTerminal);
    }
    return nonTerminalSet;
}

LL1GrammarConverter::LL1GrammarConverter(std::set<std::shared_ptr<NonTerminal>> &nonTerminalSet)
{
    for (const auto &nonTerminal : nonTerminalSet)
    {
        nonTerminals.push_back(nonTerminal);
    }
}

std::set<std::shared_ptr<NonTerminal>> LL1GrammarConverter::convertToLL1()
{
    eliminateLeftRecursion();
    performLeftFactoring();
    std::set<std::shared_ptr<NonTerminal>> nTSet = convertToSet(nonTerminals);
    return nTSet;
}

void LL1GrammarConverter::eliminateLeftRecursion()
{
    int size = (int)nonTerminals.size();
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < i; j++)
        {
            solveNonImmediateLeftRecursion(nonTerminals[i], nonTerminals[j]);
        }
        solveImmediateLeftRecursion(nonTerminals[i]);
    }
}

void LL1GrammarConverter::performLeftFactoring()
{
    for (int i = 0; i < nonTerminals.size(); i++)
    {
        factorizeNonTerminal(nonTerminals[i]);
    }
}

void LL1GrammarConverter::solveImmediateLeftRecursion(std::shared_ptr<NonTerminal> &nonTerminal)
{
    std::vector<std::vector<std::shared_ptr<Symbol>>> alphas, betas;

    // get alphas and betas
    for (const auto &production : nonTerminal->getProductions())
    {
        if (production->getFirstSymbolName() == nonTerminal->getName())
        {
            std::vector<std::shared_ptr<Symbol>> newSymbols;
            auto symbols = production->getSymbols();
            for (int i = 1; i < symbols.size(); i++)
            {
                newSymbols.push_back(symbols[i]);
            }
            alphas.push_back(newSymbols);
        }
        else
        {
            std::vector<std::shared_ptr<Symbol>> newSymbols;
            for (const auto &symbol : production->getSymbols())
            {
                newSymbols.push_back(symbol);
            }
            betas.push_back(newSymbols);
        }
    }

    if (alphas.empty())
        return;

    // create new non-terminal
    std::shared_ptr<NonTerminal> newNonTerminal = std::make_shared<NonTerminal>(nonTerminal->getName() + "'");

    // create new productions
    std::vector<std::shared_ptr<Production>> newProductions;

    for (const auto &beta : betas)
    {
        std::shared_ptr<Production> newProduction = std::make_shared<Production>();
        for (const auto &symbol : beta)
        {
            if (symbol->getName() != EPSILON)
            {
                newProduction->addSymbol(symbol);
            }
        }
        newProduction->addSymbol(newNonTerminal);
        newProductions.push_back(newProduction);
    }

    if (betas.empty())
    {
        std::shared_ptr<Production> newProduction = std::make_shared<Production>();
        newProduction->addSymbol(newNonTerminal);
        newProductions.push_back(newProduction);
    }

    nonTerminal->setProductions(newProductions);

    // create new productions for new non-terminal
    newProductions.clear();
    for (const auto &alpha : alphas)
    {
        std::shared_ptr<Production> newProduction = std::make_shared<Production>();
        for (const auto &symbol : alpha)
        {
            if (symbol->getName() != EPSILON)
            {
                newProduction->addSymbol(symbol);
            }
        }
        newProduction->addSymbol(newNonTerminal);
        newProductions.push_back(newProduction);
    }
    // add epsilon production
    std::shared_ptr<Production> epsilonProduction = std::make_shared<Production>();
    epsilonProduction->addSymbol(std::make_shared<Terminal>(EPSILON, true));
    newProductions.push_back(epsilonProduction);

    newNonTerminal->setProductions(newProductions);
    nonTerminals.push_back(newNonTerminal);
}

void LL1GrammarConverter::solveNonImmediateLeftRecursion(std::shared_ptr<NonTerminal> &nonTerminalI, std::shared_ptr<NonTerminal> &nonTerminalJ)
{
    std::vector<std::shared_ptr<Production>> newProductions;
    auto productionsJ = nonTerminalJ->getProductions();

    for (const auto &productionI : nonTerminalI->getProductions())
    {
        if (productionI->getFirstSymbolName() == nonTerminalJ->getName())
        {
            auto symbolsI = productionI->getSymbols();
            for (const auto &productionJ : productionsJ)
            {
                std::shared_ptr<Production> newProduction = std::make_shared<Production>();
                auto symbolsJ = productionJ->getSymbols();
                // append all symbols of productionJ
                for (const auto &symbol : symbolsJ)
                {
                    if (symbol->getName() != EPSILON)
                    {
                        newProduction->addSymbol(symbol);
                    }
                }
                // append all symbols of productionI except the first symbol
                for (int i = 1; i < symbolsI.size(); i++)
                {
                    newProduction->addSymbol(symbolsI[i]);
                }
                if (newProduction->getSymbols().empty())
                {
                    newProduction->addSymbol(std::make_shared<Terminal>(EPSILON, true));
                }
                newProductions.push_back(newProduction);
            }
        }
        else
        {
            newProductions.push_back(productionI);
        }
    }

    nonTerminalI->setProductions(newProductions);
}

void LL1GrammarConverter::factorizeNonTerminal(const std::shared_ptr<NonTerminal> &nonTerminal)
{
    //    nonTerminal->sortProductions();
    if (!nonTerminal)
    {
        return;
    }

    auto productions = nonTerminal->getProductions();

    std::cout << "Non-terminal: " << nonTerminal->getName() << std::endl;

    std::map<std::string, std::vector<std::shared_ptr<Production>>> productionMap;
    for (const auto &production : productions)
    {
        productionMap[production->getFirstSymbolName()].push_back(production); 
    }

    std::vector<std::shared_ptr<Production>> newProductions;
    std::string lastName = nonTerminal->getName();
    std::vector<std::shared_ptr<NonTerminal>> newNonTerminals;

    for (const auto &[_, commonProductions] : productionMap)
    {
        if (commonProductions.size() > 1)
        {
            auto newNonTerminal = std::make_shared<NonTerminal>(lastName + "*");
            lastName += "*";
            for (const auto &common : commonProductions)
            {
                std::shared_ptr<Production> newProduction = std::make_shared<Production>();
                auto symbols = common->getSymbols();
                for (int i = 1; i < symbols.size(); i++)  
                {
                    newProduction->addSymbol(symbols[i]);
                }
                if(symbols.size() == 1)
                {
                    std::shared_ptr<Terminal> eps = std::make_shared<Terminal>(EPSILON, true);
                    newProduction->addSymbol(eps);
                }
                    
                // if (symbols.size() > 1)
                // {
                    newNonTerminal->addProduction(newProduction);
                // }
            }
            newNonTerminals.push_back(newNonTerminal);
            //            nonTerminals.push_back(newNonTerminal);
            auto symbol = commonProductions[0]->getSymbols()[0];
            std::shared_ptr<Production> newProductionForBaseNonTerminal = std::make_shared<Production>();
            newProductionForBaseNonTerminal->addSymbol(symbol);
            newProductionForBaseNonTerminal->addSymbol(newNonTerminal);
            newProductions.push_back(newProductionForBaseNonTerminal);
        }
        else
        {
            newProductions.push_back(commonProductions[0]);
        }
    }

    nonTerminal->setProductions(newProductions);
    for (auto &a : newNonTerminals)
    {
        nonTerminals.push_back(a);
    }
}
