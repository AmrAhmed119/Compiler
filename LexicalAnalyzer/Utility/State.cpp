#include "State.h"

// Constructor
State::State(bool isStartingState, int priority, std::string tokenClass)
    : _isStartingState(isStartingState), _priority(priority), _tokenClass(tokenClass) {}

// Add a transition to the state
void State::addTransition(int input, std::shared_ptr<State> nextState) {
    _transitions[input].push_back(nextState);
}

// Get next states for a specific input
const std::vector<std::shared_ptr<State>>& State::getNextStates(int input) const {
    static const std::vector<std::shared_ptr<State>> empty;
    auto it = _transitions.find(input);
    return (it != _transitions.end()) ? it->second : empty;
}

// Get all transitions
const std::unordered_map<int, std::vector<std::shared_ptr<State>>>& State::getTransitions() const {
    return _transitions;
}

// Getters
bool State::isStarting() const { return _isStartingState; }
bool State::isAccepting() const { return _priority >= 0; }
int State::getPriority() const { return _priority; }
std::string State::getTokenClass() const { return _tokenClass; }

// Setters
void State::setPriority(int priority) {_priority = priority;}
void State::setStartingState(bool isStartingState) { _isStartingState = isStartingState; }
void State::setTokenClass(std::string tokenClass) { _tokenClass = tokenClass; }

