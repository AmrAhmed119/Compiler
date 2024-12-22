#ifndef PROJECT_NONTERMINAL_H
#define PROJECT_NONTERMINAL_H


#include <vector>
#include <set>
#include <map>
#include "Symbol.h"
#include "Production.h"
#include "Terminal.h"

class NonTerminal : public Symbol {
private:
    std::vector<Production> _productions;  // A vector of productions
    std::vector<std::set<Terminal>> _first; // First sets for each production
    std::set<Terminal> _follow; // Follow set for the non-terminal
    std::map<Terminal, Production> _transitions; // Transition map for terminals to production

public:
    // Constructor to initialize NonTerminal with a name
    explicit NonTerminal(const std::string& name);

    // Constructor to initialize NonTerminal with a name and a set of productions
    NonTerminal(const std::string& name, const std::vector<Production>& productions);

    // Getter for productions
    const std::vector<Production>& getProductions() const;

    // Setter for productions
    void setProductions(const std::vector<Production>& productions);

    // Getter for first set
    const std::vector<std::set<Terminal>>& getFirst() const;

    // Setter for first set
    void setFirst(const std::vector<std::set<Terminal>>& first);

    // Getter for follow set
    const std::set<Terminal>& getFollow() const;

    // Setter for follow set
    void setFollow(const std::set<Terminal>& follow);

    // Getter for transitions map
    const std::map<Terminal, Production>& getTransitions() const;

    // Setter for transitions map
    void setTransitions(const std::map<Terminal, Production>& transitions);

    // Method to add a production
    void addProduction(const Production& production);

    // Method to add a transition
    void addTransition(const Terminal& terminal, const Production& production);
};


#endif //PROJECT_NONTERMINAL_H
