//
// Created by Hp on 11/27/2024.
//

#ifndef PROJECT_STATE_H
#define PROJECT_STATE_H

#include <vector>
#include <unordered_map>

class State {
private:
    bool _isStartingState;
    int _priority;
    std::unordered_map<int, std::vector<State>> _transitions;

public:
    // Constructor
    explicit State(bool isStartingState = false, int priority = 0);

    // Methods to manage transitions
    void addTransition(int input, const State& nextState);
    const std::vector<State>& getNextStates(int input) const;
    const std::unordered_map<int, std::vector<State>>& getTransitions() const;

    // Getters
    bool isStarting() const;
    int getPriority() const;

    // Setters
    void setPriority(int priority);
    void setStartingState(bool isStartingState);
};

#endif //PROJECT_STATE_H
