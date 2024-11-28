
#ifndef PROJECT_DFA_H
#define PROJECT_DFA_H

#include "../Utility/State.h"
#include <vector>
#include <unordered_map>

class IDFA {
public:
    virtual ~IDFA() = default;
    // Create the DFA from the NFA root state
    virtual void createDFA(const State& root) = 0;
    // Minimize the DFA
    virtual std::unordered_map<State&, std::vector<State&>> minimizeDFA() = 0;
};

class DFA: public IDFA {
    State* _DFAroot;
public:
    ~DFA() override = default;
    void createDFA(State* root);
    std::unordered_map<State&, std::vector<State&>> minimizeDFA();
};


#endif //PROJECT_DFA_H
