#include "DFA.h"
#include <stack>
#include <set>
#include <memory>
#include <queue>
#include <map>

std::vector<std::shared_ptr<State>> combineStates(std::shared_ptr<State> root);
std::vector<std::shared_ptr<State>> computeEpsClosure(std::vector<std::shared_ptr<State>> &epsTransStates);
void addTransitions(std::vector<std::shared_ptr<State>> epsTransStates, std::set<std::shared_ptr<State>> &epsClosureSet, std::stack<std::shared_ptr<State>> &statesStack);
void processEpsClosure(std::set<std::shared_ptr<State>> &epsClosureSet, std::stack<std::shared_ptr<State>> &statesStack);
std::shared_ptr<State> createGragh(std::set<int> vocab, std::queue<std::vector<std::shared_ptr<State>>> &unVisited, std::map<std::vector<std::shared_ptr<State>>, std::shared_ptr<State>> &marked);
std::shared_ptr<State> createDFAState(std::vector<std::shared_ptr<State>> &dfaState);
std::vector<std::shared_ptr<State>> move(std::vector<std::shared_ptr<State>> &compinedState, int input);

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
