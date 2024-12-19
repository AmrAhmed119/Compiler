#include "DFA.h"
#include <stack>
#include <iostream>
#include <set>
#include <memory>
#include <queue>
#include <map>
#include <fstream>
#include <filesystem>
#include <climits>

std::vector<std::shared_ptr<State>> combineStates(std::shared_ptr<State> root);
std::vector<std::shared_ptr<State>> computeEpsClosure(std::vector<std::shared_ptr<State>> &epsTransStates);
void addTransitions(std::vector<std::shared_ptr<State>> epsTransStates, std::set<std::shared_ptr<State>> &epsClosureSet, std::stack<std::shared_ptr<State>> &statesStack);
void processEpsClosure(std::set<std::shared_ptr<State>> &epsClosureSet, std::stack<std::shared_ptr<State>> &statesStack);
std::shared_ptr<State> createGragh(std::set<int> vocab, std::queue<std::vector<std::shared_ptr<State>>> &unVisited, std::map<std::vector<std::shared_ptr<State>>, std::shared_ptr<State>> &marked);
std::shared_ptr<State> createDFAState(std::vector<std::shared_ptr<State>> &dfaState);
std::vector<std::shared_ptr<State>> move(std::vector<std::shared_ptr<State>> &compinedState, int input);

// Helper Functions (Minimization Part)
void traverseGraph(const std::shared_ptr<State> &root, std::unordered_set<std::shared_ptr<State>> &visited);
std::unordered_map<std::shared_ptr<State>, int> mapStatesToGroup(const std::vector<std::vector<std::shared_ptr<State>>> &groups);
std::vector<std::vector<std::shared_ptr<State>>> computeNewGroups(const std::vector<std::vector<std::shared_ptr<State>>> &groups, const std::unordered_map<std::shared_ptr<State>, int> &stateToGroup);
std::vector<int> computeStateTransitionsByGroupIds(const std::shared_ptr<State> &state, const std::unordered_map<std::shared_ptr<State>, int> &stateToGroup);
std::unordered_set<std::shared_ptr<State>> createNewDFAFromGroups(const std::vector<std::vector<std::shared_ptr<State>>> &groups, std::unordered_map<std::shared_ptr<State>, int>);
void printTransitionTable(std::unordered_set<std::shared_ptr<State>> states);

int eps;

void DFA::createDFA(std::shared_ptr<State> root, std::set<int> vocab)
{
    std::queue<std::vector<std::shared_ptr<State>>> unVisited;
    std::map<std::vector<std::shared_ptr<State>>, std::shared_ptr<State>> marked;
    eps = DFA::_epsCode;
    std::vector compinedState = combineStates(root);
    unVisited.push(compinedState);
    vocab.erase(eps);
    DFA::_DFAroot = createGragh(vocab, unVisited, marked);
}

std::vector<std::shared_ptr<State>> combineStates(std::shared_ptr<State> root)
{
    std::vector<std::shared_ptr<State>> epsTransStates;
    if (root->getTransitions().count(eps))
        epsTransStates = root->getTransitions().at(eps);
    epsTransStates.push_back(root);
    return computeEpsClosure(epsTransStates);
}

std::vector<std::shared_ptr<State>> computeEpsClosure(std::vector<std::shared_ptr<State>> &epsTransStates)
{
    std::set<std::shared_ptr<State>> epsClosureSet;
    std::stack<std::shared_ptr<State>> statesStack;
    addTransitions(epsTransStates, epsClosureSet, statesStack);
    processEpsClosure(epsClosureSet, statesStack);
    return std::vector<std::shared_ptr<State>>(epsClosureSet.begin(), epsClosureSet.end());
}

void addTransitions(std::vector<std::shared_ptr<State>> epsTransStates, std::set<std::shared_ptr<State>> &epsClosureSet, std::stack<std::shared_ptr<State>> &statesStack)
{
    for (std::shared_ptr<State> epsTransState : epsTransStates)
        if (epsClosureSet.find(epsTransState) == epsClosureSet.end())
        {
            epsClosureSet.insert(epsTransState);
            statesStack.push(epsTransState);
        }
}

void processEpsClosure(std::set<std::shared_ptr<State>> &epsClosureSet, std::stack<std::shared_ptr<State>> &statesStack)
{
    while (!statesStack.empty())
    {
        std::shared_ptr<State> state = statesStack.top();
        statesStack.pop();
        if (state->getTransitions().count(eps))
            addTransitions(state->getTransitions().at(eps), epsClosureSet, statesStack);
    }
}

std::shared_ptr<State> createGragh(std::set<int> vocab, std::queue<std::vector<std::shared_ptr<State>>> &unVisited, std::map<std::vector<std::shared_ptr<State>>, std::shared_ptr<State>> &marked)
{
    std::shared_ptr<State> root = createDFAState(unVisited.front());
    std::queue<std::shared_ptr<State>> statesStore;
    statesStore.push(root);
    while (!unVisited.empty())
    {
        std::vector<std::shared_ptr<State>> compinedState = unVisited.front();
        unVisited.pop();
        std::shared_ptr<State> curDFAState = statesStore.front();
        marked[compinedState] = curDFAState;
        for (int input : vocab)
        {
            std::vector<std::shared_ptr<State>> movedState = move(compinedState, input);
            if (marked.find(movedState) == marked.end())
            {
                unVisited.push(movedState);
                std::shared_ptr<State> next = createDFAState(movedState);
                curDFAState->addTransition(input, next);
                statesStore.push(next);
            }
            else
                curDFAState->addTransition(input, marked[movedState]);
        }
        statesStore.pop();
    }
    return root;
}

std::shared_ptr<State> createDFAState(std::vector<std::shared_ptr<State>> &dfaState)
{
    bool isStarting = false;
    int priority = INT_MAX;
    std::string className = "";
    for (std::shared_ptr<State> state : dfaState)
    {
        // assume higher priority has larger int value
        if (state->getPriority() != -1 && state->getPriority() < priority)
        {
            priority = state->getPriority();
            className = state->getTokenClass();
        }
        isStarting = isStarting || state->isStarting();
    }
    if (priority == INT_MAX)
        priority = -1;
    std::shared_ptr<State> newState = std::make_shared<State>(isStarting, priority, className);
    return newState;
}

std::vector<std::shared_ptr<State>> move(std::vector<std::shared_ptr<State>> &compinedState, int input)
{
    std::vector<std::shared_ptr<State>> newDfaState;
    for (std::shared_ptr<State> state : compinedState)
    {
        std::vector<std::shared_ptr<State>> nextStates = state->getNextStates(input);
        for (std::shared_ptr<State> nextState : nextStates)
            newDfaState.push_back(nextState);
    }
    return computeEpsClosure(newDfaState);
}

// Minimization Part

// print the result minimization table to a file
void printTransitionTable(std::unordered_set<std::shared_ptr<State>> states) {
    std::ofstream outputFile("../LexicalAnalyzer/Outputs/transitionTable.txt", std::ios::out);
    if (!outputFile) {
        std::cerr << "Error: Unable to open output file \"transition_table\"." << std::endl;
        return;
    }

    // create mapping for each state to its name
    std::map<std::shared_ptr<State>, int> stateToName;
    int i = 0, start = 0;
    for (const auto &state: states) {
        stateToName[state] = i++;
        if (state->isStarting()) {
            start = i - 1;
        }
    }

    outputFile << "Transition table for the minimized DFA:\n\n";
    outputFile << "Start Node: " << start << "\n\n";
    outputFile << std::left << std::setw(20) << "State" << std::setw(20) << "Transition" << std::setw(20)
               << "Next State"
               << std::setw(20) << "Token Class" << "\n";
    for (const auto &state: states) {
        for (const auto &transition: state->getTransitions()) {
            outputFile << std::left << std::setw(20) << stateToName[state] << std::setw(20) << transition.first
                       << std::setw(20) << stateToName[transition.second[0]] << std::setw(20)
                       << transition.second[0]->getTokenClass() << "\n";
        }
    }
}

// Main DFA Minimalization
std::unordered_set<std::shared_ptr<State>> DFA::minimizeDFA()
{

    // STEP 1: Traverse the graph and get all the states
    std::unordered_set<std::shared_ptr<State>> visited;
    traverseGraph(_DFAroot, visited);

    // STEP 2: Split the nodes into two groups accepting and non-accepting
    std::vector<std::shared_ptr<State>> nonAcceptingStates;
    std::map<std::string, std::vector<std::shared_ptr<State>>> acceptingStatesMap;

    for (const auto &state : visited)
    {
        if (state->isAccepting()) {
            acceptingStatesMap[state->getTokenClass()].push_back(state);
        }
    }

    std::set<std::string> uniqueClassesAccepting;
    for (const auto &state : visited)
    {
        if (!state->isAccepting())
        {
            nonAcceptingStates.push_back(state);
        }
    }
    std::vector<std::vector<std::shared_ptr<State>>> groups = {nonAcceptingStates};

    for (const auto &[_, states] : acceptingStatesMap)
    {
        groups.push_back(states);
    }


    // STEP 3: Split the nodes into groups based on their transitions (Main Algorithm)
    while (true)
    {
        // map each state with its group id
        auto stateToGroup = mapStatesToGroup(groups);

        // start splitting the states into groups
        auto newGroups = computeNewGroups(groups, stateToGroup);

        // check if the groups are the same, if yes then break
        if (newGroups.size() == groups.size())
        {
            break;
        }

        // update the groups
        groups = newGroups;
    }

    // STEP 4: Create the new DFA (new State represent each group)
    auto stateToGroup = mapStatesToGroup(groups);

    auto newDFAStates = createNewDFAFromGroups(groups, stateToGroup);

    printTransitionTable(newDFAStates);

    return newDFAStates;
}

// Helper Function to Traverse the Graph
void traverseGraph(const std::shared_ptr<State> &root, std::unordered_set<std::shared_ptr<State>> &visited)
{
    if (visited.find(root) != visited.end())
    {
        return;
    }
    visited.insert(root);

    for (const auto &[input, nextStates] : root->getTransitions())
    {
        for (const auto &nextState : nextStates)
        {
            traverseGraph(nextState, visited);
        }
    }
}

// Helper Function to Map States to Groups
std::unordered_map<std::shared_ptr<State>, int> mapStatesToGroup(const std::vector<std::vector<std::shared_ptr<State>>> &groups)
{
    std::unordered_map<std::shared_ptr<State>, int> stateToGroup;
    for (int i = 0; i < groups.size(); i++)
    {
        for (const auto &state : groups[i])
        {
            stateToGroup[state] = i;
        }
    }
    return stateToGroup;
}

// Helper Function to Compute State Transitions Group Ids
std::vector<int> computeStateTransitionsByGroupIds(const std::shared_ptr<State> &state, const std::unordered_map<std::shared_ptr<State>, int> &stateToGroup)
{
    auto stateTransitions = state->getTransitions();
    std::vector<int> stateTransitionsGroupIds;

    for (const auto &[input, nextStates] : stateTransitions)
    {
        stateTransitionsGroupIds.push_back(stateToGroup.at(nextStates[0]));
    }

    return stateTransitionsGroupIds;
}

// Helper Function to Compute New Groups
std::vector<std::vector<std::shared_ptr<State>>> computeNewGroups(const std::vector<std::vector<std::shared_ptr<State>>> &groups, const std::unordered_map<std::shared_ptr<State>, int> &stateToGroup)
{
    std::vector<std::vector<std::shared_ptr<State>>> newGroups;

    for (const auto &group : groups)
    {
        std::map<std::vector<int>, std::vector<std::shared_ptr<State>>> commonTransitions;
        for (const auto &state : group)
        {
            std::vector<int> stateTransitions = computeStateTransitionsByGroupIds(state, stateToGroup);
            commonTransitions[stateTransitions].push_back(state);
        }
        for (const auto &[_, states] : commonTransitions)
        {
            std::vector<std::shared_ptr<State>> newGroup;
            for (const auto &state : states)
            {
                newGroup.push_back(state);
            }
            newGroups.push_back(newGroup);
        }
    }
    return newGroups;
}

// Helper Function to Compute New DFA from the minimized groups
std::unordered_set<std::shared_ptr<State>> createNewDFAFromGroups(const std::vector<std::vector<std::shared_ptr<State>>> &groups, std::unordered_map<std::shared_ptr<State>, int> stateToGroup)
{
    int newStatesSize = groups.size();
    std::vector<std::shared_ptr<State>> newStates(newStatesSize);

    // create a new state for each group
    for (int i = 0; i < newStatesSize; i++)
    {
        bool isStartingState = false;
        int maxPriority = INT_MAX;
        std::string tokenClass;
        for (const auto &state : groups[i])
        {
            isStartingState |= state->isStarting();
            if (state->getPriority() != -1 && state->getPriority() < maxPriority)
            {
                maxPriority = state->getPriority();
                tokenClass = state->getTokenClass();
            }
        }
        if (maxPriority == INT_MAX)
            maxPriority = -1;
        newStates[i] = std::make_shared<State>(isStartingState, maxPriority, tokenClass);
    }

    for (int i = 0; i < newStatesSize; i++)
    {
        auto groupTransitions = groups[i][0]->getTransitions();
        for (const auto &[input, nextStates] : groupTransitions)
        {
            auto nextState = nextStates[0];
            newStates[i]->addTransition(input, newStates[stateToGroup[nextState]]);
        }
    }

    std::unordered_set<std::shared_ptr<State>> finalStates;
    for (const auto &state : newStates)
    {
        finalStates.insert(state);
    }

    return finalStates;
}