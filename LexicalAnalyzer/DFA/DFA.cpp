#include "DFA.h"
#include <map>

// Helper Functions (Updated for Smart Pointers)
std::vector<std::shared_ptr<State>> createCompinedState(std::shared_ptr<State> root);
std::vector<std::shared_ptr<State>> computeEpsClosure(std::vector<std::shared_ptr<State>>& epsTransStates);
std::shared_ptr<State> createDFAState(std::vector<std::shared_ptr<State>>& dfaState);
std::vector<std::shared_ptr<State>> move(const std::vector<std::shared_ptr<State>>& compinedState, int input);
std::shared_ptr<State> createGragh(int vocabSize, std::stack<std::vector<std::shared_ptr<State>>> unVisited, std::set<std::vector<std::shared_ptr<State>>> marked);


// Helper Functions (Minimization Part)
void traverseGraph(const std::shared_ptr<State>& root, std::unordered_set<std::shared_ptr<State>>& visited);
std::unordered_map<std::shared_ptr<State>, int> mapStatesToGroup(const std::vector<std::vector<std::shared_ptr<State>>>& groups);
std::vector<std::vector<std::shared_ptr<State>>> computeNewGroups(const std::vector<std::vector<std::shared_ptr<State>>>& groups, const std::unordered_map<std::shared_ptr<State>, int>& stateToGroup);
std::vector<int> computeStateTransitionsByGroupIds(const std::shared_ptr<State>& state, const std::unordered_map<std::shared_ptr<State>, int>& stateToGroup);


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


// Minimization Part

// Main DFA Minimalization
std::unordered_set<std::shared_ptr<State>> DFA::minimizeDFA() {

    // STEP 1: Traverse the graph and get all the states
    std::unordered_set<std::shared_ptr<State>> visited;
    traverseGraph(_DFAroot, visited);

    // STEP 2: Split the nodes into two groups accepting and non-accepting
    std::vector<std::shared_ptr<State>> acceptingStates, nonAcceptingStates;
    for (const auto& state: visited) {
        if (state->isAccepting()) {
            acceptingStates.push_back(state);
        } else {
            nonAcceptingStates.push_back(state);
        }
    }

    // STEP 3: Split the nodes into groups based on their transitions (Main Algorithm)
    // Create the groups
    std::vector<std::vector<std::shared_ptr<State>>> groups = {acceptingStates, nonAcceptingStates};

    while (true) {
        // map each state with its group id
        std::unordered_map<std::shared_ptr<State>, int> stateToGroup = mapStatesToGroup(groups);

        // start splitting the states into groups
        std::vector<std::vector<std::shared_ptr<State>>> newGroups = computeNewGroups(groups, stateToGroup);

        // check if the groups are the same, if yes then break
        if (newGroups.size() == groups.size()) {
            break;
        }
    }

    // TODO STEP 4: Create the new DFA (new State represent each group)


    return {};
}

// Helper Function to Traverse the Graph
void traverseGraph(const std::shared_ptr<State>& root, std::unordered_set<std::shared_ptr<State>>& visited) {
    if (visited.find(root) != visited.end()) {
        return;
    }
    visited.insert(root);

    for (const auto& [input, nextStates] : root->getTransitions()) {
        for (const auto& nextState : nextStates) {
            traverseGraph(nextState, visited);
        }
    }
}

// Helper Function to Map States to Groups
std::unordered_map<std::shared_ptr<State>, int> mapStatesToGroup(const std::vector<std::vector<std::shared_ptr<State>>>& groups) {
    std::unordered_map<std::shared_ptr<State>, int> stateToGroup;
    for (int i = 0; i < groups.size(); i++) {
        for (const auto& state : groups[i]) {
            stateToGroup[state] = i;
        }
    }
    return stateToGroup;
}

// Helper Function to Compute State Transitions Group Ids
std::vector<int> computeStateTransitionsByGroupIds(const std::shared_ptr<State>& state, const std::unordered_map<std::shared_ptr<State>, int>& stateToGroup) {
    auto stateTransitions = state->getTransitions();
    std::vector<int> stateTransitionsGroupIds;

    for (const auto& [input, nextStates] : stateTransitions) {
        stateTransitionsGroupIds.push_back(stateToGroup.at(nextStates[0]));
    }

    return stateTransitionsGroupIds;
}

// Helper Function to Compute New Groups
std::vector<std::vector<std::shared_ptr<State>>> computeNewGroups(const std::vector<std::vector<std::shared_ptr<State>>>& groups, const std::unordered_map<std::shared_ptr<State>, int>& stateToGroup) {
    std::vector<std::vector<std::shared_ptr<State>>> newGroups;

    for (const auto& group : groups) {
        std::map<std::vector<int>, std::vector<std::shared_ptr<State>>> commonTransitions;
        for (const auto& state : group) {
            std::vector<int> stateTransitions = computeStateTransitionsByGroupIds(state, stateToGroup);
            commonTransitions[stateTransitions].push_back(state);
        }
        for (const auto& [_, states] : commonTransitions) {
            std::vector<std::shared_ptr<State>> newGroup;
            for (const auto& state : states) {
                newGroup.push_back(state);
            }
            newGroups.push_back(newGroup);
        }
    }
    return newGroups;
}