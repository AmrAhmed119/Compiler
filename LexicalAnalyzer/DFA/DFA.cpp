#include "DFA.h"
#include <stack>
#include <set>
#include <memory>

// Functions definetions are added just in case
std::vector<std::shared_ptr<State>> createCompinedState(std::shared_ptr<State> root);
std::vector<std::shared_ptr<State>> computeEpsClosure(std::vector<std::shared_ptr<State>> &epsTransStates);
void initializeEpsClosure(std::vector<std::shared_ptr<State>> &epsTransStates,
                          std::set<std::shared_ptr<State>> &epsClosureSet, std::stack<std::shared_ptr<State>> &statesStack);
void processEpsClosure(std::set<std::shared_ptr<State>> &epsClosureSet, std::stack<std::shared_ptr<State>> &statesStack);
std::vector<std::shared_ptr<State>> convertSetToVector(std::set<std::shared_ptr<State>> &epsClosureSet);
std::shared_ptr<State> createGragh(int vocabSize, std::stack<std::vector<std::shared_ptr<State>>> &unVisited, std::set<std::vector<std::shared_ptr<State>>> &marked);
std::shared_ptr<State> createDFAState(std::vector<std::shared_ptr<State>> &dfaState);
std::vector<std::shared_ptr<State>> move(std::vector<std::shared_ptr<State>> &compinedState, int input);

void DFA::createDFA(std::shared_ptr<State> root)
{
    std::stack<std::vector<std::shared_ptr<State>>> unVisited;
    std::set<std::vector<std::shared_ptr<State>>> marked;
    std::vector compinedState = createCompinedState(root);
    unVisited.push(compinedState);
    int vocabSize = root->getTransitions().size();
    DFA::_DFAroot = createGragh(vocabSize, unVisited, marked);
}

std::vector<std::shared_ptr<State>> createCompinedState(std::shared_ptr<State> root)
{
    std::vector<std::shared_ptr<State>> epsTransStates = root->getTransitions().at(0);
    epsTransStates.push_back(root);
    return computeEpsClosure(epsTransStates);
}

std::vector<std::shared_ptr<State>> computeEpsClosure(std::vector<std::shared_ptr<State>> &epsTransStates)
{
    std::set<std::shared_ptr<State>> epsClosureSet;
    std::stack<std::shared_ptr<State>> statesStack;
    initializeEpsClosure(epsTransStates, epsClosureSet, statesStack);
    processEpsClosure(epsClosureSet, statesStack);
    std::vector<std::shared_ptr<State>> vecEpsTransStates = convertSetToVector(epsClosureSet);
    return vecEpsTransStates;
}

void initializeEpsClosure(std::vector<std::shared_ptr<State>> &epsTransStates,
                          std::set<std::shared_ptr<State>> &epsClosureSet, std::stack<std::shared_ptr<State>> &statesStack)
{
    for (std::shared_ptr<State> state : epsTransStates)
    {
        epsClosureSet.insert(state);
        statesStack.push(state);
    }
}

void processEpsClosure(std::set<std::shared_ptr<State>> &epsClosureSet, std::stack<std::shared_ptr<State>> &statesStack)
{
    while (!statesStack.empty())
    {
        std::shared_ptr<State> state = statesStack.top();
        statesStack.pop();
        // Assume epsilon transitions are at index 0 in the transitions map
        for (std::shared_ptr<State> epsTransState : state->getTransitions().at(0))
        {
            if (!epsClosureSet.count(epsTransState))
            {
                epsClosureSet.insert(epsTransState);
                statesStack.push(epsTransState);
            }
        }
    }
}

std::vector<std::shared_ptr<State>> convertSetToVector(std::set<std::shared_ptr<State>> &epsClosureSet)
{
    return std::vector<std::shared_ptr<State>>(epsClosureSet.begin(), epsClosureSet.end());
}

std::shared_ptr<State> createGragh(int vocabSize, std::stack<std::vector<std::shared_ptr<State>>> &unVisited, std::set<std::vector<std::shared_ptr<State>>> &marked)
{
    std::shared_ptr<State> root;
    bool rootInit = false;
    while (!unVisited.empty())
    {
        std::vector<std::shared_ptr<State>> compinedState = unVisited.top();
        unVisited.pop();
        marked.insert(compinedState);
        std::shared_ptr<State> curDFAState = createDFAState(compinedState);
        for (int input = 1; input < vocabSize; input++)
        {
            std::vector<std::shared_ptr<State>> movedState = move(compinedState, input);
            std::vector<std::shared_ptr<State>> newDfaState = computeEpsClosure(movedState);
            if (!marked.count(newDfaState))
                unVisited.push(newDfaState);
            curDFAState->addTransition(input, createDFAState(newDfaState));
        }
        if (!rootInit)
        {
            rootInit = true;
            root = curDFAState;
        }
        return root;
    }
}

std::shared_ptr<State> createDFAState(std::vector<std::shared_ptr<State>> &dfaState)
{
    bool isStarting = false;
    int priority = -1;
    for (std::shared_ptr<State> state : dfaState)
    {
        // assume higher priority has larger int value
        priority = std::max(priority, state->getPriority());
        isStarting = isStarting || state->isStarting();
    }
    std::shared_ptr<State> newState = std::make_shared<State>(isStarting, priority);
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
