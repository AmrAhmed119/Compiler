#include <iostream>
#include "LexicalAnalyzer/Tokenizer/FileReader.h"
#include "LexicalAnalyzer/Tokenizer/Tokenizer.h"
#include "LexicalAnalyzer/DFA/DFA.h"

const std::string filePath = "E:\\CSED25\\4th year\\First Term\\Compilers\\Project\\LexicalAnalyzer\\Inputs\\program.txt";

int main() {

    std::unordered_set<std::shared_ptr<State>> states;
    std::shared_ptr<State> state1 = std::make_shared<State>(true, -1, "");
    std::shared_ptr<State> state2 = std::make_shared<State>(false, 1, "Int");
    std::shared_ptr<State> state3 = std::make_shared<State>(false, 2, "Amr");
    std::shared_ptr<State> state4 = std::make_shared<State>(false, 3, "Ahmed");

    state1->addTransition('a', state2);
    state1->addTransition('b', state3);
    state1->addTransition('c', state4);

    DFA dfa;
    dfa.DFAroot = state1;

    states = dfa.minimizeDFA();

    // create the tokenizer with the file and the states
    Tokenizer tokenizer(filePath, states);

    while (tokenizer.hasMoreTokens()) {
        std::cout << tokenizer.getNextToken() << "\n";
    }

    std::cout << "----------------Tokens---------------" << "\n";

    for (const auto& token : tokenizer.getTokens()) {
        std::cout << token.getValue() << "   " << token.getType() << "\n";
    }

    return 0;
}
