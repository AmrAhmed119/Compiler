#include <iostream>
#include <memory>
#include "../Utility/State.cpp"
#include "DFA.cpp"

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

int main()
{
    std::shared_ptr<State> root = createDummyNFA();
    DFA *dfa = new DFA();
    dfa->createDFA(root, 3);
    std::queue<std::shared_ptr<State>> s;
    s.push(dfa->_DFAroot);
    while (!s.empty())
    {
        std::shared_ptr<State> cur = s.front();
        s.pop();
        std::cout<<"Priority = "<<cur->getPriority()<<" is starting = "<<cur->isStarting()<<"\n";
        for(auto states :cur->getTransitions())
            for(auto state :states.second)
                s.push(state);
    }
    return 0;
}
