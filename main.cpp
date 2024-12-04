#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include "LexicalAnalyzer/NFA/process_tokens.cpp"
void printNFA(std::shared_ptr<State> root)
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
    // Example input regex: "a|b"
    std::string regex = "a|b";
    std::string regex2 = "((a|b)(a|b|0)*)";

    try
    {
        // Convert infix regex to postfix
        std::string postfixRegex = infixToPostfix(regex);
        printf("Postfix regex: %s\n", postfixRegex.c_str());

        // Call the Thompson construction function with the postfix regex
        NFA result = convertToNFA(regex);
        NFA result2 = regexToNFA(postfixRegex);
        std::shared_ptr<State> root = result2.startStates[0];
        printNFA(root);

        // Assuming you have methods in the NFA class to print the NFA's states and transitions
        //result2.printNFA(); // This method would print out the NFA's states and transitions
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

