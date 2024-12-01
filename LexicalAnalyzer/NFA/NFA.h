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

class NFA
{
public:

    std::stack<NFA> nfaStack;                           // Stack to hold NFAs
    std::vector<std::shared_ptr<State>> startStates;    // Start states
    std::vector<std::shared_ptr<State>> endStates;      // End states
    std::unordered_map<int, std::shared_ptr<State>> states; // Map of states


    NFA(); // Constructor

    // Stack operations
    NFA pop();                 // Pop top NFA from stack
    void push(const NFA &nfa); // Push NFA to stack

    // NFA operations
    void concatenate(NFA &nfa1);            // Concatenate current NFA with another
    void concatenateAllStack();            // Concatenate all NFAs on the stack
    void orOp(NFA &nfa1);                  // Perform OR operation with another NFA
    void kleeneStar();                     // Apply Kleene Star operation
    void positiveClosure();                // Apply positive closure (1+ repetitions)
    void processSymbol(const int &symbol); // Create NFA for a single symbol

    // State and transition management
    std::shared_ptr<State> createState(bool isAccepting = false, bool isStarting = false, const std::string &token = ""); // Create a state
    void addTransition(std::shared_ptr<State> from, const int &symbol, std::shared_ptr<State> to);                          // Add a transition

    // Epsilon closure
    std::unordered_map<std::shared_ptr<State>, std::set<std::shared_ptr<State>>,
            std::hash<std::shared_ptr<State>>, std::equal_to<std::shared_ptr<State>>>
    getEpsilonClosureSetMap();                                                                              // Compute epsilon closure map
    std::set<std::shared_ptr<State>> computeEpsilonClosure(const std::set<std::shared_ptr<State>> &states); // Compute epsilon closure for a set of states

    // Debugging and visualization
    void printNFA() const; // Print the NFA
    void printStateTransitions(const std::shared_ptr<State> &state, std::unordered_set<std::shared_ptr<State>> &visitedStates, std::unordered_map<std::shared_ptr<State>, int> &stateToNumber, int &stateCounter) const;
    void printStateTransitions(const std::shared_ptr<State> &state, std::unordered_set<std::shared_ptr<State>> &visitedStates) const;

    // Utility functions
    void mergeStates(const NFA &otherNFA); // Merge states from another NFA

};

#endif // NFA_H
