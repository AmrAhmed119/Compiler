#include "NonTerminal.h"

// Constructor to initialize NonTerminal with a name
NonTerminal::NonTerminal(const std::string& name, bool isStarting) : Symbol(name), isStarting(isStarting) {}

// Constructor to initialize NonTerminal with a name and a set of productions
NonTerminal::NonTerminal(const std::string& name, const std::vector<std::shared_ptr<Production>>& productions, bool isStarting)
        : Symbol(name), _productions(productions), isStarting(isStarting) {}

// Getter for productions
const std::vector<std::shared_ptr<Production>> &NonTerminal::getProductions() const
{
    return _productions;
}

// Setter for productions
void NonTerminal::setProductions(const std::vector<std::shared_ptr<Production>> &productions)
{
    _productions = productions;
}

// Getter for first set
const std::vector<std::set<std::shared_ptr<Terminal>>> &NonTerminal::getFirst() const
{
    return _first;
}

// Setter for first set
void NonTerminal::setFirst(const std::vector<std::set<std::shared_ptr<Terminal>>> &first)
{
    _first = first;
}

// Getter for follow set
const std::set<std::shared_ptr<Terminal>> &NonTerminal::getFollow() const
{
    return _follow;
}

// Setter for follow set
void NonTerminal::setFollow(const std::set<std::shared_ptr<Terminal>> &follow)
{
    _follow = follow;
}

// Getter for transitions map
const std::map<std::shared_ptr<Terminal>, std::shared_ptr<Production>> &NonTerminal::getTransitions() const
{
    return _transitions;
}

// Getter for isStarting
bool NonTerminal::getIsStarting() const {
    return isStarting;
}

// Setter for isStarting
void NonTerminal::setIsStarting(bool starting) {
    isStarting = starting;
}

// Setter for transitions map
void NonTerminal::setTransitions(const std::map<std::shared_ptr<Terminal>, std::shared_ptr<Production>> &transitions)
{
    _transitions = transitions;
}

// Method to add a production
void NonTerminal::addProduction(const std::shared_ptr<Production> &production)
{
    _productions.push_back(production);
}

// Method to add a transition
void NonTerminal::addTransition(const std::shared_ptr<Terminal> &terminal, const std::shared_ptr<Production> &production)
{
    _transitions[terminal] = production;
}

void NonTerminal::sortProductions() {
    std::sort(_productions.begin(), _productions.end(),
    [](const std::shared_ptr<Production> &a, const std::shared_ptr<Production> &b) {
        // Extract symbols from each production
        const auto &symbolsA = a->getSymbols();
        const auto &symbolsB = b->getSymbols();

        // Convert symbols to their names
        std::vector<std::string> namesA;
        std::vector<std::string> namesB;

        for (const auto &symbol : symbolsA)
            namesA.push_back(symbol->getName()); // Assuming Symbol has a getName() method.
        for (const auto &symbol : symbolsB)
            namesB.push_back(symbol->getName());

        // Compare the names lexicographically
        return namesA < namesB; // std::vector supports lexicographical comparison
    });
}
