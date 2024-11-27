
#include "DFA.h"
#include <stack>
#include <set>

void DFA::createDFA(const State &root)
{
    std::stack<std::vector<State>> unVisited;
    std::set<std::vector<State>> marked;
    std::vector dfaState = createDfaState(root);
    unVisited.push(dfaState);
    int vocabSize = root.getTransitions().size();
    while (!unVisited.empty())
    {
        std::vector<State> dfaState = unVisited.top();
        unVisited.pop();
        marked.insert(dfaState);
        for (int input = 1; input <= vocabSize; input++)
        {
            std::vector<State> newDfaState = computeEpsClosure(move(dfaState, input));
            if (!marked.count(newDfaState))
                unVisited.push(newDfaState);
        }
    }
}

std::vector<State> &move(std::vector<State> &dfaState, int input)
{
    std::vector<State> newDfaState;
    for (State state : dfaState)
    {
        std::vector<State> nextStates = state.getNextStates(input);
        for (State nextState : nextStates)
            newDfaState.push_back(nextState);
    }
    return computeEpsClosure(newDfaState);
}

std::vector<State> &createDfaState(const State &root)
{
    std::vector<State> epsTransStates = root.getTransitions().at(0);
    epsTransStates.push_back(root);
    return computeEpsClosure(epsTransStates);
}

std::vector<State> &computeEpsClosure(std::vector<State> &epsTransStates)
{
    std::set<State> epsClosureSet;
    std::stack<State> statesStack;
    initializeEpsClosure(epsTransStates, epsClosureSet, statesStack);
    processEpsClosure(epsClosureSet, statesStack);
    std::vector<State> vecEpsTransStates = convertSetToVector(epsClosureSet);
    return vecEpsTransStates;
}

void initializeEpsClosure(const std::vector<State> &epsTransStates,
                          std::set<State> &epsClosureSet, std::stack<State> &statesStack)
{
    for (const State &state : epsTransStates)
    {
        epsClosureSet.insert(state);
        statesStack.push(state);
    }
}

void processEpsClosure(std::set<State> &epsClosureSet, std::stack<State> &statesStack)
{
    while (!statesStack.empty())
    {
        State state = statesStack.top();
        statesStack.pop();
        // Assume epsilon transitions are at index 0 in the transitions map
        for (const State &epsTransState : state.getTransitions().at(0))
        {
            if (!epsClosureSet.count(epsTransState))
            {
                epsClosureSet.insert(epsTransState);
                statesStack.push(epsTransState);
            }
        }
    }
}

std::vector<State> convertSetToVector(const std::set<State> &epsClosureSet)
{
    return std::vector<State>(epsClosureSet.begin(), epsClosureSet.end());
}
