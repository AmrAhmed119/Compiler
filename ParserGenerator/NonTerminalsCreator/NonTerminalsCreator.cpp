#include "NonTerminalsCreator.h"
#include "../Utility/Util.h"
#include "../../LexicalAnalyzer/Utility/Util.h"

std::vector<std::set<std::shared_ptr<Terminal>>> calcFirstOfNT(std::shared_ptr<NonTerminal> nt);
std::set<std::shared_ptr<Terminal>> calcFirstOfProduction(std::shared_ptr<Production> production);
std::set<std::shared_ptr<Terminal>> setsUnion(std::vector<std::set<std::shared_ptr<Terminal>>> unexpandedSet);
bool containsEps(std::set<std::shared_ptr<Terminal>> expandedSet);

void NonTerminalsCreator::createFirst(std::map<std::string, std::shared_ptr<NonTerminal>> &nonTerminals)
{
    for (auto nt : nonTerminals)
    {
        std::shared_ptr<NonTerminal> cur = nt.second;
        std::vector<std::set<std::shared_ptr<Terminal>>> firsts = calcFirstOfNT(cur);
        cur->setFirst(firsts);
    }
}

std::vector<std::set<std::shared_ptr<Terminal>>> calcFirstOfNT(std::shared_ptr<NonTerminal> nt)
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

std::set<std::shared_ptr<Terminal>> calcFirstOfProduction(std::shared_ptr<Production> production)
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
            if (!containsEps(expandedSet))
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

std::set<std::shared_ptr<Terminal>> setsUnion(std::vector<std::set<std::shared_ptr<Terminal>>> unexpandedSet)
{
    std::set<std::shared_ptr<Terminal>> expandedSet;
    for (std::set<std::shared_ptr<Terminal>> terminlsSet : unexpandedSet)
        expandedSet.insert(terminlsSet.begin(), terminlsSet.end());
    return expandedSet;
}

bool containsEps(std::set<std::shared_ptr<Terminal>> expandedSet)
{
    for (std::shared_ptr<Terminal> terminal : expandedSet)
        if (terminal->getIsEpsilon())
            return true;
    return false;
}

void NonTerminalsCreator::createFollow(std::map<std::string, std::shared_ptr<NonTerminal>> &nonTerminals)
{
}
