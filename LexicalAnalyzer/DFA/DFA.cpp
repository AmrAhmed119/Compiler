
#include "DFA.h"
#include <stack>
#include <set>

// Functions definetions are added just in case
std::vector<State *> createCompinedState(State *root);
std::vector<State *> computeEpsClosure(std::vector<State *> &epsTransStates);
void initializeEpsClosure(std::vector<State *> &epsTransStates,
                          std::set<State *> &epsClosureSet, std::stack<State *> &statesStack);
void processEpsClosure(std::set<State *> &epsClosureSet, std::stack<State *> &statesStack);
std::vector<State *> convertSetToVector(std::set<State *> &epsClosureSet);
State *createGragh(int vocabSize, std::stack<std::vector<State *>> unVisited, std::set<std::vector<State *>> marked);
State *createDFAState(std::vector<State *> &dfaState);
std::vector<State *> move(std::vector<State *> &compinedState, int input);

void DFA::createDFA(State *root)
{
    std::stack<std::vector<State *>> unVisited;
    std::set<std::vector<State *>> marked;
    std::vector compinedState = createCompinedState(root);
    unVisited.push(compinedState);
    int vocabSize = root->getTransitions().size();
    DFA::_DFAroot = createGragh(vocabSize, unVisited, marked);
}

std::vector<State *> createCompinedState(State *root)
{
    std::vector<State *> epsTransStates = root->getTransitions().at(0);
    epsTransStates.push_back(root);
    return computeEpsClosure(epsTransStates);
}

std::vector<State *> computeEpsClosure(std::vector<State *> &epsTransStates)
{
    std::set<State *> epsClosureSet;
    std::stack<State *> statesStack;
    initializeEpsClosure(epsTransStates, epsClosureSet, statesStack);
    processEpsClosure(epsClosureSet, statesStack);
    std::vector<State *> vecEpsTransStates = convertSetToVector(epsClosureSet);
    return vecEpsTransStates;
}

void initializeEpsClosure(std::vector<State *> &epsTransStates,
                          std::set<State *> &epsClosureSet, std::stack<State *> &statesStack)
{
    for (State *state : epsTransStates)
    {
        epsClosureSet.insert(state);
        statesStack.push(state);
    }
}

void processEpsClosure(std::set<State *> &epsClosureSet, std::stack<State *> &statesStack)
{
    while (!statesStack.empty())
    {
        State *state = statesStack.top();
        statesStack.pop();
        // Assume epsilon transitions are at index 0 in the transitions map
        for (State *epsTransState : state->getTransitions().at(0))
        {
            if (!epsClosureSet.count(epsTransState))
            {
                epsClosureSet.insert(epsTransState);
                statesStack.push(epsTransState);
            }
        }
    }
}

std::vector<State *> convertSetToVector(std::set<State *> &epsClosureSet)
{
    return std::vector<State *>(epsClosureSet.begin(), epsClosureSet.end());
}

State *createGragh(int vocabSize, std::stack<std::vector<State *>> unVisited, std::set<std::vector<State *>> marked)
{
    State *root;
    bool rootInit = false;
    while (!unVisited.empty())
    {
        std::vector<State *> compinedState = unVisited.top();
        unVisited.pop();
        marked.insert(compinedState);
        State *curDFAState = createDFAState(compinedState);
        for (int input = 1; input < vocabSize; input++)
        {
            std::vector<State *> movedState = move(compinedState, input);
            std::vector<State *> newDfaState = computeEpsClosure(movedState);
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

State *createDFAState(std::vector<State *> &dfaState)
{
    bool isStarting = false;
    int priority = -1;
    for (State *state : dfaState)
    {
        // assume higher priority has larger int value
        priority = std::max(priority, state->getPriority());
        isStarting = isStarting || state->isStarting();
    }
    State *newState = new State(isStarting, priority);
    return newState;
}

std::vector<State *> move(const std::vector<State *> &compinedState, int input)
{
    std::vector<State *> newDfaState;
    for (State *state : compinedState)
    {
        std::vector<State *> nextStates = state->getNextStates(input);
        for (State *nextState : nextStates)
            newDfaState.push_back(nextState);
    }
    return computeEpsClosure(newDfaState);
}
