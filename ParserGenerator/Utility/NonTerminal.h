#ifndef PROJECT_NONTERMINAL_H
#define PROJECT_NONTERMINAL_H

#include <vector>
#include <set>
#include <map>
#include "Symbol.h"
#include "Production.h"
#include "Terminal.h"

class NonTerminal : public Symbol
{
private:
    std::vector<std::shared_ptr<Production>> _productions;                         // A vector of productions
    std::vector<std::set<std::shared_ptr<Terminal>>> _first;                       // First sets for each production
    std::set<std::shared_ptr<Terminal>> _follow;                                   // Follow set for the non-terminal
    std::map<std::shared_ptr<Terminal>, std::shared_ptr<Production>> _transitions; // Transition map for terminals to production
    bool isStarting; // Attribute to store whether the non-terminal is the starting non-terminal or not

public:
    // Constructor to initialize NonTerminal with a name
    explicit NonTerminal(const std::string& name, bool isStarting = false);

    // Constructor to initialize NonTerminal with a name and a set of productions
    NonTerminal(const std::string& name, const std::vector<std::shared_ptr<Production>>& productions, bool isStarting);

    // Getter for productions
    const std::vector<std::shared_ptr<Production>> &getProductions() const;

    // Setter for productions
    void setProductions(const std::vector<std::shared_ptr<Production>> &productions);

    // Getter for first set
    const std::vector<std::set<std::shared_ptr<Terminal>>> &getFirst() const;

    // Getter for isStarting
    bool getIsStarting() const;

    // Setter for isStarting
    void setIsStarting(bool starting);

    // Getter for isStarting
    bool getIsStarting() const;

    // Setter for isStarting
    void setIsStarting(bool starting);

    // Setter for first set
    void setFirst(const std::vector<std::set<std::shared_ptr<Terminal>>> &first);

    // Getter for follow set
    const std::set<std::shared_ptr<Terminal>> &getFollow() const;

    // Setter for follow set
    void setFollow(const std::set<std::shared_ptr<Terminal>> &follow);

    // Getter for transitions map
    const std::map<std::shared_ptr<Terminal>, std::shared_ptr<Production>> &getTransitions() const;

    // Setter for transitions map
    void setTransitions(const std::map<std::shared_ptr<Terminal>, std::shared_ptr<Production>> &transitions);

    // Method to add a production
    void addProduction(const std::shared_ptr<Production> &production);

    // Method to add a transition
    void addTransition(const std::shared_ptr<Terminal> &terminal, const std::shared_ptr<Production> &production);
};

#endif // PROJECT_NONTERMINAL_H