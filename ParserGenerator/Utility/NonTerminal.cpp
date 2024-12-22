#include "NonTerminal.h"

// Constructor to initialize NonTerminal with a name
NonTerminal::NonTerminal(const std::string &name) : Symbol(name) {}

// Constructor to initialize NonTerminal with a name and a set of productions
NonTerminal::NonTerminal(const std::string &name, const std::vector<std::shared_ptr<Production>> &productions)
    : Symbol(name), _productions(productions) {}

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
const std::set < std::shared_ptr<Terminal >> &NonTerminal::getFollow() const
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

// Setter for transitions map
void NonTerminal::setTransitions(const std::map<std::shared_ptr<Terminal>, std::shared_ptr<Production>> &transitions)
{
    _transitions = transitions;
}

// Method to add a production
void NonTerminal::addProduction(std::shared_ptr<Production> production)
{
    _productions.push_back(production);
}

// Method to add a transition
void NonTerminal::addTransition(std::shared_ptr<Terminal> terminal, std::shared_ptr<Production> production)
{
    _transitions[terminal] = production;
}