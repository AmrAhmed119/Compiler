#include <iostream>
#include "LexicalAnalyzer/Tokenizer/Tokenizer.h"
#include "LexicalAnalyzer/DFA/DFA.h"

const std::string ex1FilePath = "/home/ahmed/level4term1/compilers/Project/Compiler/LexicalAnalyzer/Inputs/program.txt";
const std::string ex2FilePath = "/home/ahmed/level4term1/compilers/Project/Compiler/LexicalAnalyzer/Inputs/program2.txt";
const std::string ex3FilePath = "/home/ahmed/level4term1/compilers/Project/Compiler/LexicalAnalyzer/Inputs/program3.txt";

std::shared_ptr<State> oneAcceptDummyNFA()
{
    std::vector<std::shared_ptr<State>> states;
    states.push_back(std::make_shared<State>(true, -1, ""));
    for (int i = 0; i < 9; i++)
        states.push_back(std::make_shared<State>(false, -1, ""));
    states.push_back(std::make_shared<State>(false, 1, "abb"));
    states[0]->addTransition(0, states[1]);
    states[0]->addTransition(0, states[7]);

    states[1]->addTransition(0, states[2]);
    states[1]->addTransition(0, states[4]);

    states[2]->addTransition('1', states[3]);

    states[3]->addTransition(0, states[6]);

    states[4]->addTransition('2', states[5]);

    states[5]->addTransition(0, states[6]);

    states[6]->addTransition(0, states[7]);
    states[6]->addTransition(0, states[1]);

    states[7]->addTransition('1', states[8]);

    states[8]->addTransition('2', states[9]);

    states[9]->addTransition('2', states[10]);
    return states[0];
}

std::shared_ptr<State> minimizableDummyNFA(){
    std::vector<std::shared_ptr<State>> states;
    states.push_back(std::make_shared<State>(false, -1, ""));
    states.push_back(std::make_shared<State>(false, -1, ""));
    states.push_back(std::make_shared<State>(false, -1, ""));
    states.push_back(std::make_shared<State>(false, 1, "d"));
    states.push_back(std::make_shared<State>(true, -1, ""));

    states[0]->addTransition('0', states[1]);
    states[0]->addTransition('0', states[2]);
    states[0]->addTransition('1', states[2]);
    states[0]->addTransition('1', states[3]);

    states[1]->addTransition('1', states[3]);

    states[2]->addTransition('0', states[0]);
    states[2]->addTransition('0', states[1]);
    states[2]->addTransition('1', states[2]);

    states[3]->addTransition('1', states[0]);
    states[3]->addTransition('1', states[2]);

    states[4]->addTransition('e', states[0]);
    states[4]->addTransition('e', states[1]);

    return states[4];
}

std::shared_ptr<State> multiAccept(){
    std::vector<std::shared_ptr<State>> states;
    states.push_back(std::make_shared<State>(true, -1, ""));
    states.push_back(std::make_shared<State>(false, 1, "b"));
    states.push_back(std::make_shared<State>(false, 2, "c"));
    states.push_back(std::make_shared<State>(false, -1, ""));

    states[0]->addTransition('0', states[1]);
    states[0]->addTransition('0', states[2]);

    states[1]->addTransition('1', states[2]);
    states[1]->addTransition('1', states[0]);

    states[2]->addTransition('0', states[3]);
    states[2]->addTransition('1', states[3]);

    states[3]->addTransition('0', states[1]);
    states[3]->addTransition('1', states[3]);

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
            std::cout << nextStates.first << "-----> ";
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

std::unordered_set<std::shared_ptr<State>> example1Accept(){
    std::shared_ptr<State> root = oneAcceptDummyNFA();
    DFA *dfa = new DFA();
    std::set<int> s;
    s.insert(0);
    s.insert('1');
    s.insert('2');
    dfa->_epsCode = 0;
    dfa->createDFA(root, s);
    // print(dfa->_DFAroot);
    return dfa->minimizeDFA();
}

std::unordered_set<std::shared_ptr<State>> minimizableExample(){
    std::shared_ptr<State> root = minimizableDummyNFA();
    DFA *dfa = new DFA();
    std::set<int> s;
    s.insert('e');
    s.insert('0');
    s.insert('1');
    dfa->_epsCode = 'e';
    dfa->createDFA(root, s);
    // print(dfa->_DFAroot);
    return dfa->minimizeDFA();
}

std::unordered_set<std::shared_ptr<State>> multiAcceptExample(){
    std::shared_ptr<State> root = multiAccept();
    DFA *dfa = new DFA();
    std::set<int> s;
    s.insert('e');
    s.insert('0');
    s.insert('1');
    dfa->_epsCode = 'e';
    dfa->createDFA(root, s);
    // print(dfa->_DFAroot);
    return dfa->minimizeDFA();
}

void getTokenizerResults(std::string filePath, std::unordered_set<std::shared_ptr<State>> states){
    Tokenizer tokenizer(filePath, states);
    while (tokenizer.hasMoreTokens()) {
        std::cout << tokenizer.getNextToken() << "\n";
    }

    std::cout << "----------------Tokens---------------" << "\n";

    for (const auto& token : tokenizer.getTokens()) {
        std::cout << "Token: " << token.getValue() << "      Class: " << token.getType() << "\n";
    }
}

int main() {
    std::unordered_set<std::shared_ptr<State>> states;
    states = example1Accept();
    getTokenizerResults(ex1FilePath, states);
    states = minimizableExample();
    getTokenizerResults(ex2FilePath, states);
    states = multiAcceptExample();
    getTokenizerResults(ex3FilePath, states);
    return 0;
}
