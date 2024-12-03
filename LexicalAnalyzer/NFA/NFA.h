#ifndef NFA_H
#define NFA_H

#include <iostream>
#include <unordered_map>
#include <stack>
#include <vector>
#include <set>
#include <memory>
#include "../Utility/State.h"
#include <unordered_set>

class NFA {
public:
    // State management
    std::unordered_set<std::shared_ptr<State>> states;    // Use a set for unique states
    std::shared_ptr<State> startState;                     // Single start state
    std::unordered_set<std::shared_ptr<State>> endStates;  // Multiple end states

    NFA(); // Constructor

    // Stack operations (if required)
    void push(const NFA &nfa); // Push NFA to stack
    NFA pop();                 // Pop top NFA from stack

    // NFA operations
    void concatenate(NFA &nfa1); // Concatenate current NFA with another
    void orOp(NFA &nfa1);        // Perform OR operation with another NFA
    void kleeneStar();           // Apply Kleene Star operation
    void positiveClosure();      // Apply positive closure (1+ repetitions)
    void processSymbol(const int &symbol); // Create NFA for a single symbol

    // Transition management
    void addTransition(std::shared_ptr<State> from, const int &symbol, std::shared_ptr<State> to);
    
    // Epsilon closure operations
    std::set<std::shared_ptr<State>> computeEpsilonClosure(const std::set<std::shared_ptr<State>> &states);

    // Debugging and visualization
    void printNFA() const;  // Print the NFA
    void printStateTransitions(const std::shared_ptr<State> &state) const; // Print state transitions

private:
    // Helper methods for merging and managing states
    void mergeStates(NFA &otherNFA); // Merge states from another NFA
    void addEpsilonTransition(std::shared_ptr<State> from, std::shared_ptr<State> to);
    std::unordered_map<std::shared_ptr<State>, std::set<std::shared_ptr<State>>> getEpsilonClosureSetMap() const;
};

#endif // NFA_H
