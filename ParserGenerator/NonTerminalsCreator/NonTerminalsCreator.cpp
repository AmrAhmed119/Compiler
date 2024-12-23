#include "NonTerminalsCreator.h"
#include "../Utility/Util.h"
#include "../../LexicalAnalyzer/Utility/Util.h"
#include <iostream>

std::vector<std::set<std::shared_ptr<Terminal>>> calcFirstOfNT(std::shared_ptr<NonTerminal> &nt);
std::set<std::shared_ptr<Terminal>> calcFirstOfProduction(std::shared_ptr<Production> &production);
std::set<std::shared_ptr<Terminal>> setsUnion(const std::vector<std::set<std::shared_ptr<Terminal>>> &unexpandedSet);
bool hasEps(const std::set<std::shared_ptr<Terminal>> &expandedSet);
void addTerminalsAndFirst(std::map<std::string, std::shared_ptr<NonTerminal>> &nonTerminals);
void addDollarForStart(std::shared_ptr<NonTerminal> &nonTerminal);
void terminalAndFirstToProduction(std::shared_ptr<Production> &production);
bool terminalAndFirstToNt(std::shared_ptr<NonTerminal> &cur, std::shared_ptr<Symbol> &next);
void removeEpsFromSet(std::set<std::shared_ptr<Terminal>> &curSet);
void addFollows(std::map<std::string, std::shared_ptr<NonTerminal>> &nonTerminals);
void addFollowForProduction(std::shared_ptr<Production> &production, std::shared_ptr<NonTerminal> &head);
void followToNt(std::shared_ptr<NonTerminal> &nonTerminal, std::shared_ptr<NonTerminal> &head);

void NonTerminalsCreator::createFirst(std::map<std::string, std::shared_ptr<NonTerminal>> &nonTerminals)
{
    for (auto nt : nonTerminals)
    {
        std::shared_ptr<NonTerminal> cur = nt.second;
        std::vector<std::set<std::shared_ptr<Terminal>>> firsts = calcFirstOfNT(cur);
        cur->setFirst(firsts);
    }
}

std::vector<std::set<std::shared_ptr<Terminal>>> calcFirstOfNT(std::shared_ptr<NonTerminal> &nt)
{
    std::vector<std::set<std::shared_ptr<Terminal>>> firstsOfNt;
    if (!nt->getFirst().empty())
        firstsOfNt = nt->getFirst();
    else
        for (std::shared_ptr<Production> production : nt->getProductions())
        {
            std::set<std::shared_ptr<Terminal>> firstOfProduction = calcFirstOfProduction(production);
            firstsOfNt.push_back(firstOfProduction);
        }
    return firstsOfNt;
}

std::set<std::shared_ptr<Terminal>> calcFirstOfProduction(std::shared_ptr<Production> &production)
{
    std::set<std::shared_ptr<Terminal>> firstOfProduction;

    for (std::shared_ptr<Symbol> symbol : production->getSymbols())
    {
        // Use std::dynamic_pointer_cast for casting shared_ptr
        if (std::shared_ptr<Terminal> terminal = std::dynamic_pointer_cast<Terminal>(symbol))
        {
            firstOfProduction.insert(terminal);
            break;
        }
        else if (std::shared_ptr<NonTerminal> nonTerminal = std::dynamic_pointer_cast<NonTerminal>(symbol))
        {
            std::vector<std::set<std::shared_ptr<Terminal>>> unexpandedSet = calcFirstOfNT(nonTerminal);
            std::set<std::shared_ptr<Terminal>> expandedSet = setsUnion(unexpandedSet);
            firstOfProduction.insert(expandedSet.begin(), expandedSet.end());

            // If expandedSet does not contain epsilon, break the loop
            if (!hasEps(expandedSet))
                break;
        }
        else
        {
            // Handle invalid symbol types
            print("Wrong symbol type");
        }
    }

    return firstOfProduction;
}

std::set<std::shared_ptr<Terminal>> setsUnion(const std::vector<std::set<std::shared_ptr<Terminal>>> &unexpandedSet)
{
    std::set<std::shared_ptr<Terminal>> expandedSet;
    for (std::set<std::shared_ptr<Terminal>> terminlsSet : unexpandedSet)
        expandedSet.insert(terminlsSet.begin(), terminlsSet.end());
    return expandedSet;
}

bool hasEps(const std::set<std::shared_ptr<Terminal>> &expandedSet)
{
    for (std::shared_ptr<Terminal> terminal : expandedSet)
        if (terminal->getIsEpsilon())
            return true;
    return false;
}

void NonTerminalsCreator::createFollow(std::map<std::string, std::shared_ptr<NonTerminal>> &nonTerminals)
{
    addTerminalsAndFirst(nonTerminals);
    addFollows(nonTerminals);
}

void addTerminalsAndFirst(std::map<std::string, std::shared_ptr<NonTerminal>> &nonTerminals)
{
    for (auto nonTerminal : nonTerminals)
    {
        addDollarForStart(nonTerminal.second);
        for (std::shared_ptr<Production> production : nonTerminal.second->getProductions())
            terminalAndFirstToProduction(production);
    }
}

void addDollarForStart(std::shared_ptr<NonTerminal> &nonTerminal)
{
    if (nonTerminal->getIsStarting())
    {
        std::set<std::shared_ptr<Terminal>> curSet;
        std::shared_ptr<Terminal> endText = std::make_shared<Terminal>("$", false);
        curSet.insert(endText);
        nonTerminal->setFollow(curSet);
    }
}

void terminalAndFirstToProduction(std::shared_ptr<Production> &production)
{
    std::vector<std::shared_ptr<Symbol>> symbols = production->getSymbols();
    int numSymbols = symbols.size();
    for (int symbIndx = 0; symbIndx < numSymbols; symbIndx++)
    {
        if (std::shared_ptr<NonTerminal> nonTerminal = std::dynamic_pointer_cast<NonTerminal>(symbols[symbIndx]))
        {
            int next = symbIndx + 1;
            bool hasNext = true;
            while (next != numSymbols && hasNext)
                hasNext = terminalAndFirstToNt(nonTerminal, symbols[next++]);
        }
    }
}

bool terminalAndFirstToNt(std::shared_ptr<NonTerminal> &cur, std::shared_ptr<Symbol> &next)
{
    std::set<std::shared_ptr<Terminal>> curSet = cur->getFollow();
    bool ans = false;
    removeEpsFromSet(curSet);
    if (std::shared_ptr<Terminal> terminal = std::dynamic_pointer_cast<Terminal>(next))
    {
        curSet.insert(terminal);
        ans = false;
    }
    else
    {
        std::shared_ptr<NonTerminal> nonTerminal = std::dynamic_pointer_cast<NonTerminal>(next);
        std::set<std::shared_ptr<Terminal>> nextFirst = setsUnion(nonTerminal->getFirst());
        curSet.insert(nextFirst.begin(), nextFirst.end());
        if (hasEps(nextFirst))
            ans = true;
        else
            ans = false;
    }
    cur->setFollow(curSet);
    return ans;
}

void removeEpsFromSet(std::set<std::shared_ptr<Terminal>> &curSet)
{
    for (std::shared_ptr<Terminal> terminal : curSet)
        if (terminal->getIsEpsilon())
        {
            curSet.erase(terminal);
            break;
        }
}

void addFollows(std::map<std::string, std::shared_ptr<NonTerminal>> &nonTerminals)
{
    for (auto nonTerminal : nonTerminals)
    {
        addDollarForStart(nonTerminal.second);
        for (std::shared_ptr<Production> production : nonTerminal.second->getProductions())
            addFollowForProduction(production, nonTerminal.second);
    }
}

void addFollowForProduction(std::shared_ptr<Production> &production, std::shared_ptr<NonTerminal> &head)
{
    std::vector<std::shared_ptr<Symbol>> symbols = production->getSymbols();
    int numSymbols = symbols.size();
    for (int symbIndx = 0; symbIndx < numSymbols; symbIndx++)
    {
        if (std::shared_ptr<NonTerminal> nonTerminal = std::dynamic_pointer_cast<NonTerminal>(symbols[symbIndx]))
            if (symbIndx == numSymbols - 1 || hasEps(nonTerminal->getFollow()))
                followToNt(nonTerminal, head);
    }
}

void followToNt(std::shared_ptr<NonTerminal> &nonTerminal, std::shared_ptr<NonTerminal> &head)
{
    std::set<std::shared_ptr<Terminal>> curSet = nonTerminal->getFollow();
    removeEpsFromSet(curSet);
    curSet.insert(head->getFollow().begin(), head->getFollow().end());
    nonTerminal->setFollow(curSet);
}
