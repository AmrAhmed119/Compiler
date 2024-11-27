//
// Created by Hp on 11/27/2024.
//

#include "State.h"

// Constructor
State::State(bool isStartingState, int priority)
        : _isStartingState(isStartingState), _priority(priority) {}

// Add a transition
void State::addTransition(int input, const State& nextState) {
    _transitions[input].push_back(nextState);
}

// Get the next states for a specific input
const std::vector<State>& State::getNextStates(int input) const {
    static const std::vector<State> empty;
    auto it = _transitions.find(input);
    return it != _transitions.end() ? it->second : empty;
}

// Get all transitions
const std::unordered_map<int, std::vector<State>>& State::getTransitions() const {
    return _transitions;
}

// Check if this is the starting state
bool State::isStarting() const {
    return _isStartingState;
}

// Get the priority of the state
int State::getPriority() const {
    return _priority;
}

// Set the priority
void State::setPriority(int priority) {
    _priority = priority;
}

// Set whether this is the starting state
void State::setStartingState(bool isStartingState) {
    _isStartingState = isStartingState;
}

