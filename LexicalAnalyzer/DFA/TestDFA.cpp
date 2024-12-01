#include <iostream>
#include <memory>
#include "../Utility/State.cpp"
#include "DFA.cpp"
#include <set>

std::shared_ptr<State> createDummyNFA()
{
    std::vector<std::shared_ptr<State>> states;
    states.push_back(std::make_shared<State>(true, -1));
    for (int i = 0; i < 9; i++)
        states.push_back(std::make_shared<State>(false, -1));
    states.push_back(std::make_shared<State>(false, 1));
    states[0]->addTransition(0, states[1]);
    states[0]->addTransition(0, states[7]);

    states[1]->addTransition(0, states[2]);
    states[1]->addTransition(0, states[4]);

    states[2]->addTransition(1, states[3]);

    states[3]->addTransition(0, states[6]);

    states[4]->addTransition(2, states[5]);

    states[5]->addTransition(0, states[6]);

    states[6]->addTransition(0, states[7]);
    states[6]->addTransition(0, states[1]);

    states[7]->addTransition(1, states[8]);

    states[8]->addTransition(2, states[9]);

    states[9]->addTransition(2, states[10]);
    return states[0];
}

void print(std::shared_ptr<State> root)
{
    std::queue<std::shared_ptr<State>> s;
    std::set<std::shared_ptr<State>> uq;
    s.push(root);
    uq.insert(root);

    while (!s.empty())
    {
        std::shared_ptr<State> cur = s.front();
        s.pop();
        std::cout << cur << "       ";
        for (auto nextStates : cur->getTransitions())
        {
            std::cout<<nextStates.first<<"-----> ";
            for (auto state : nextStates.second)
            {
                std::cout << state << "(" << state->getPriority() << ", " << state->isStarting() << ")" << ",";
                if (uq.find(state) == uq.end())
                {
                    uq.insert(state);
                    s.push(state);
                }
            }
            std::cout << "        ";
        }
        std::cout << "\n";
    }
}

int main()
{
    std::shared_ptr<State> root = createDummyNFA();
    DFA *dfa = new DFA();
    dfa->createDFA(root, 3);
    print(dfa->_DFAroot);

    return 0;
}
