#ifndef PROJECT_DFA_H
#define PROJECT_DFA_H

#include <stack>
#include <set>
#include <vector>
#include <memory>
#include <unordered_set>
#include "../Utility/State.h"

class IDFA {
public:
    virtual ~IDFA() = default;
    virtual void createDFA(std::shared_ptr<State> root) = 0;
    virtual std::unordered_set<std::shared_ptr<State>> minimizeDFA() = 0;
};

class DFA: public IDFA {
    std::shared_ptr<State> _DFAroot;

public:
    ~DFA() = default;
    void createDFA(std::shared_ptr<State> root) override;
    std::unordered_set<std::shared_ptr<State>> minimizeDFA() override;
};

#endif // PROJECT_DFA_H