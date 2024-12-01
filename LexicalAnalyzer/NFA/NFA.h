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
    NFA(); // Constructor

    NFA pop();                 // Pop top NFA from stack
    void push(const NFA &nfa); // Push NFA to stack

    void concatenate(NFA &nfa1);

    // void concatenate(NFA &otherNFA);

    void orOp(NFA &nfa1);

    // void concatenate();                            // Concatenate top two NFAs on stack
    void concatenateAllStack();                    // Concatenate all NFAs on stack
    // void orOp();                                   // Perform OR operation on top two NFAs
    void kleeneStar();                             // Kleene Star operation
    void positiveClosure();                        // Positive closure operation (1+ repetitions)
    void processSymbol(const int &symbol); // Create NFA for a single symbol

    std::shared_ptr<State> createState(bool isAccepting = false, bool isStarting = false, const std::string &token = ""); // Add a state
    void addTransition(std::shared_ptr<State> from, const int &symbol, std::shared_ptr<State> to);                // Add a transition

    std::unordered_map<std::shared_ptr<State>, std::set<std::shared_ptr<State>>,
                       std::hash<std::shared_ptr<State>>, std::equal_to<std::shared_ptr<State>>>
    getEpsilonClosureSetMap();                                                                              // Compute epsilon closure map
    std::set<std::shared_ptr<State>> computeEpsilonClosure(const std::set<std::shared_ptr<State>> &states); // Compute epsilon closure
    void printNFA() const;
    void printStateTransitions(const std::shared_ptr<State> &state, std::unordered_set<std::shared_ptr<State>> &visitedStates, std::unordered_map<std::shared_ptr<State>, int> &stateToNumber, int &stateCounter) const;
    void printStateTransitions(const std::shared_ptr<State> &state, std::unordered_set<std::shared_ptr<State>> &visitedStates) const;
    std::stack<NFA> nfaStack; // Stack to hold NFAs

    std::vector<std::shared_ptr<State>> startStates;        // Start states
    std::vector<std::shared_ptr<State>> endStates;          // End states
    std::unordered_map<int, std::shared_ptr<State>> states; // States

    void mergeStates(const NFA &otherNFA); // Merge states from another NFA
};

#endif // NFA_H
