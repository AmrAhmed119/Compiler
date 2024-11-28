#include "DFA.h"

// Helper Functions (Updated for Smart Pointers)
std::vector<std::shared_ptr<State>> createCompinedState(std::shared_ptr<State> root);
std::vector<std::shared_ptr<State>> computeEpsClosure(std::vector<std::shared_ptr<State>>& epsTransStates);
std::shared_ptr<State> createDFAState(std::vector<std::shared_ptr<State>>& dfaState);
std::vector<std::shared_ptr<State>> move(const std::vector<std::shared_ptr<State>>& compinedState, int input);

// Main DFA Creation
void DFA::createDFA(std::shared_ptr<State> root) {
    std::stack<std::vector<std::shared_ptr<State>>> unVisited;
    std::set<std::vector<std::shared_ptr<State>>> marked;
    std::vector<std::shared_ptr<State>> combinedState = createCompinedState(root);
    unVisited.push(combinedState);
    int vocabSize = root->getTransitions().size();
    _DFAroot = createGragh(vocabSize, unVisited, marked);
}

// Graph Creation
std::shared_ptr<State> createGragh(int vocabSize, std::stack<std::vector<std::shared_ptr<State>>> unVisited, std::set<std::vector<std::shared_ptr<State>>> marked) {
    std::shared_ptr<State> root;
    bool rootInit = false;

    while (!unVisited.empty()) {
        auto combinedState = unVisited.top();
        unVisited.pop();
        marked.insert(combinedState);
        auto curDFAState = createDFAState(combinedState);

        for (int input = 1; input < vocabSize; input++) {
            auto movedState = move(combinedState, input);
            auto newDfaState = computeEpsClosure(movedState);
            if (!marked.count(newDfaState))
                unVisited.push(newDfaState);
            curDFAState->addTransition(input, createDFAState(newDfaState));
        }

        if (!rootInit) {
            rootInit = true;
            root = curDFAState;
        }
    }

    return root;
}

// Compute Epsilon Closure
std::vector<std::shared_ptr<State>> computeEpsClosure(std::vector<std::shared_ptr<State>>& epsTransStates) {
    std::set<std::shared_ptr<State>> epsClosureSet;
    std::stack<std::shared_ptr<State>> statesStack;

    for (auto& state : epsTransStates) {
        epsClosureSet.insert(state);
        statesStack.push(state);
    }

    while (!statesStack.empty()) {
        auto state = statesStack.top();
        statesStack.pop();
        for (auto& epsTransState : state->getTransitions().at(0)) {
            if (!epsClosureSet.count(epsTransState)) {
                epsClosureSet.insert(epsTransState);
                statesStack.push(epsTransState);
            }
        }
    }

    return {epsClosureSet.begin(), epsClosureSet.end()};
}

// Create DFA State
std::shared_ptr<State> createDFAState(std::vector<std::shared_ptr<State>>& dfaState) {
    bool isStarting = false;
    int priority = -1;

    for (const auto& state : dfaState) {
        priority = std::max(priority, state->getPriority());
        isStarting = isStarting || state->isStarting();
    }

    return std::make_shared<State>(isStarting, priority);
}

// Move Function
std::vector<std::shared_ptr<State>> move(const std::vector<std::shared_ptr<State>>& combinedState, int input) {
    std::vector<std::shared_ptr<State>> newDfaState;

    for (const auto& state : combinedState) {
        auto nextStates = state->getNextStates(input);
        newDfaState.insert(newDfaState.end(), nextStates.begin(), nextStates.end());
    }

    return computeEpsClosure(newDfaState);
}
