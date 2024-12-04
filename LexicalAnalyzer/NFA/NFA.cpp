#include "NFA.h"

// Constructor for NFA
NFA::NFA() {
    states.clear();
    startState = nullptr;
    endStates.clear();
}

// Push an NFA onto the stack
void NFA::push(const NFA &nfa) {
    throw std::logic_error("Stack operations are not defined in this structure.");
}

// Pop an NFA from the stack
NFA NFA::pop() {
    throw std::logic_error("Stack operations are not defined in this structure.");
}

// Add a transition between states
void NFA::addTransition(std::shared_ptr<State> from, const int &symbol, std::shared_ptr<State> to) {
    if (!from || !to) {
        throw std::invalid_argument("From or to state is null.");
    }
    from->addTransition(symbol, to);
    states.insert(from);
    states.insert(to);
}

// Merge states from another NFA
void NFA::mergeStates(NFA &otherNFA) {
    for (auto &state : otherNFA.states) {
        states.insert(state);
    }
}

// Add an epsilon transition
void NFA::addEpsilonTransition(std::shared_ptr<State> from, std::shared_ptr<State> to) {
    addTransition(from, '/l', to);  // Use '/l' to denote epsilon transitions
}

// Perform concatenation
void NFA::concatenate(NFA &nfa1) {
    for (auto &endState : endStates) {
        addEpsilonTransition(endState, nfa1.startState);
    }
    endStates = nfa1.endStates;
    mergeStates(nfa1);
}

// Perform OR operation
void NFA::orOp(NFA &nfa1) {
    auto newStart = std::make_shared<State>(true, false);
    auto newEnd = std::make_shared<State>(false, true);

    addEpsilonTransition(newStart, startState);
    addEpsilonTransition(newStart, nfa1.startState);

    for (auto &endState : endStates) {
        addEpsilonTransition(endState, newEnd);
    }

    for (auto &endState : nfa1.endStates) {
        addEpsilonTransition(endState, newEnd);
    }

    startState = newStart;
    endStates = {newEnd};
    mergeStates(nfa1);
    states.insert(newStart);
    states.insert(newEnd);
}

// Apply Kleene Star operation
void NFA::kleeneStar() {
    auto newStart = std::make_shared<State>(true, false);
    auto newEnd = std::make_shared<State>(false, true);

    addEpsilonTransition(newStart, startState);
    for (auto &endState : endStates) {
        addEpsilonTransition(endState, newEnd);
        addEpsilonTransition(endState, startState);
    }
    addEpsilonTransition(newStart, newEnd);

    startState = newStart;
    endStates = {newEnd};
    states.insert(newStart);
    states.insert(newEnd);
}

// Apply positive closure
void NFA::positiveClosure() {
    auto newStart = std::make_shared<State>(true, false);
    auto newEnd = std::make_shared<State>(false, true);

    addEpsilonTransition(newStart, startState);
    for (auto &endState : endStates) {
        addEpsilonTransition(endState, newEnd);
        addEpsilonTransition(endState, startState);
    }

    startState = newStart;
    endStates = {newEnd};
    states.insert(newStart);
    states.insert(newEnd);
}

// Process a single symbol
void NFA::processSymbol(const int &symbol) {
    auto start = std::make_shared<State>(true, false);
    auto end = std::make_shared<State>(false, true);
    addTransition(start, symbol, end);

    startState = start;
    endStates = {end};
    states.insert(start);
    states.insert(end);
}

// Compute epsilon closure
std::set<std::shared_ptr<State>> NFA::computeEpsilonClosure(const std::set<std::shared_ptr<State>> &inputStates) {
    std::set<std::shared_ptr<State>> closure = inputStates;
    std::stack<std::shared_ptr<State>> stack;

    for (auto &state : inputStates) {
        stack.push(state);
    }

    while (!stack.empty()) {
        auto current = stack.top();
        stack.pop();

        auto transitions = current->getTransitions();
        if (transitions.find('/l') != transitions.end()) {
            for (auto &nextState : transitions['/l']) {
                if (closure.find(nextState) == closure.end()) {
                    closure.insert(nextState);
                    stack.push(nextState);
                }
            }
        }
    }

    return closure;
}

