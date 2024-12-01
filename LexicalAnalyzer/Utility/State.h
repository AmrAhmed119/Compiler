#ifndef PROJECT_STATE_H
#define PROJECT_STATE_H

#include <vector>
#include <unordered_map>
#include <memory> // For smart pointers
#include <string>

class State {
    bool _isStartingState;
    std::string _tokenClass;
    int _priority;
    std::unordered_map<int, std::vector<std::shared_ptr<State>>> _transitions;

public:
    // Constructor
    explicit State(bool isStartingState = false, int priority = 0);

    // Methods to manage transitions
    void addTransition(int input, std::shared_ptr<State> nextState);
    const std::vector<std::shared_ptr<State>>& getNextStates(int input) const;
    const std::unordered_map<int, std::vector<std::shared_ptr<State>>>& getTransitions() const;

    // Getters
    bool isStarting() const;
    bool isAccepting() const;
    int getPriority() const;
    std::string getTokenClass() const;


    // Setters
    void setPriority(int priority);
    void setStartingState(bool isStartingState);
    std::string setTokenClass(const std::string &tokenClass);
};

#endif // PROJECT_STATE_H
