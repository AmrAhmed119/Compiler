#ifndef PROJECT_DFA_H
#define PROJECT_DFA_H

#include <stack>
#include <set>
#include <vector>
#include <memory>
#include <unordered_map>
#include "State.h"

class DFA {
    std::shared_ptr<State> _DFAroot;

public:
    ~DFA() = default;
    void createDFA(std::shared_ptr<State> root);
    std::unordered_map<std::shared_ptr<State>, std::vector<std::shared_ptr<State>>> minimizeDFA();
};

#endif // PROJECT_DFA_H
