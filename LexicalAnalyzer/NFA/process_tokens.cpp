#include <iostream>
#include <stack>
#include <cassert>
#include <algorithm>
#include "NFA.h"
#include "../Utility/State.h"


// this function for converting postfix to NFA ( for id )
NFA regexToNFA(const std::string& postfix) {
    std::stack<NFA> nfaStack;

    for (size_t i = 0; i < postfix.size(); ++i) {
        char r = postfix[i];

        switch (r) {
            case '|': { // Union
                if (nfaStack.size() < 2) {
                    throw std::runtime_error("Not enough NFAs for union.");
                }

                auto nfa2 = nfaStack.top(); nfaStack.pop();
                auto nfa1 = nfaStack.top(); nfaStack.pop();

                NFA newNFA;
                auto newStart = std::make_shared<State>();
                auto newEnd = std::make_shared<State>();

                newNFA.addTransition(newStart, '/l', nfa1.startStates.front());
                newNFA.addTransition(newStart, '/l', nfa2.startStates.front());

                for (const auto& end : nfa1.endStates) {
                    newNFA.addTransition(end, '/l', newEnd);
                }
                for (const auto& end : nfa2.endStates) {
                    newNFA.addTransition(end, '/l', newEnd);
                }

                newNFA.startStates.push_back(newStart);
                newNFA.endStates.push_back(newEnd);

                nfaStack.push(newNFA);
                break;
            }

            case '*': { // Kleene star
                if (nfaStack.empty()) {
                    throw std::runtime_error("Not enough NFAs for '*'.");
                }

                auto nfa = nfaStack.top(); nfaStack.pop();

                NFA newNFA;
                auto newStart = std::make_shared<State>();
                auto newEnd = std::make_shared<State>();

                newNFA.addTransition(newStart, '/l', nfa.startStates.front());
                newNFA.addTransition(newStart, '/l', newEnd);

                for (const auto& end : nfa.endStates) {
                    newNFA.addTransition(end, '/l', nfa.startStates.front());
                    newNFA.addTransition(end, '/l', newEnd);
                }

                newNFA.startStates.push_back(newStart);
                newNFA.endStates.push_back(newEnd);

                nfaStack.push(newNFA);
                break;
            }

            default: { // Literal characters (no concatenation)
                NFA newNFA;

                auto initial = std::make_shared<State>();
                auto accept = std::make_shared<State>();

                newNFA.addTransition(initial, r, accept);

                newNFA.startStates.push_back(initial);
                newNFA.endStates.push_back(accept);

                nfaStack.push(newNFA);
                break;
            }
        }
    }

    // if (nfaStack.size() != 1) {
    //     throw std::runtime_error("Invalid postfix expression.");
    // }

    return nfaStack.top();
}




int precedence(char op)
{
    if (op == '|')
        return 1;
    if (op == '.')
        return 2; // Concatenation
    if (op == '*')
        return 3; // Kleene star
    return 0;
}

std::string infixToPostfix(const std::string &infix)
{
    std::stack<char> operators;
    std::string postfix;

    for (char c : infix)
    {
        if (isalnum(c))
        {
            // Operand (a character, digit, etc.), add to postfix
            postfix += c;
        }
        else if (c == '(')
        {
            // Left parenthesis, push to stack
            operators.push(c);
        }
        else if (c == ')')
        {
            // Right parenthesis, pop from stack to postfix until left parenthesis
            while (!operators.empty() && operators.top() != '(')
            {
                postfix += operators.top();
                operators.pop();
            }
            operators.pop(); // Discard the '('
        }
        else if (c == '|' || c == '.' || c == '*')
        {
            // Operator: pop from stack to postfix based on precedence
            while (!operators.empty() && precedence(operators.top()) >= precedence(c))
            {
                postfix += operators.top();
                operators.pop();
            }
            operators.push(c);
        }
    }

    // Pop all remaining operators from the stack
    while (!operators.empty())
    {
        postfix += operators.top();
        operators.pop();
    }

    return postfix;
}


// this to find nfa for simple words (all before id)
NFA convertToNFA(const std::string& input) {
    // Step 1: Remove backslashes from the input
    std::string cleanedInput;
    for (char c : input) {
        if (c != '\\') {  // Skip backslashes
            cleanedInput += c;
        }
    }

    // Step 2: Reverse the cleaned string
    std::reverse(cleanedInput.begin(), cleanedInput.end());

    // Step 3: Create NFA for each character and push it onto the stack
    std::stack<NFA> nfaStack;
    for (char c : cleanedInput) {
        NFA nfa;
        nfa.processSymbol(c);  // Process the symbol to create the NFA for the character
        nfaStack.push(nfa);    // Push the NFA for the character onto the stack
    }

    // Step 4: Concatenate all the NFAs in the stack
    NFA finalNFA;  // Start with an uninitialized finalNFA

    // Pop the first NFA to initialize finalNFA
    if (!nfaStack.empty()) {
        finalNFA = nfaStack.top();  // Initialize with the first NFA
        nfaStack.pop();
    }

    // Concatenate remaining NFAs
    while (!nfaStack.empty()) {
        NFA nfa = nfaStack.top();
        nfaStack.pop();
        finalNFA.concatenate(nfa);  // Concatenate the current NFA with finalNFA
    }

    return finalNFA;  // Return the final concatenated NFA
}



int main()
{
    // Example input regex: "a|b"
    std::string regex = "abc";
    std::string regex2 = "((a|b)(a|b|0)*)";

    try
    {
        // Convert infix regex to postfix
        std::string postfixRegex = infixToPostfix(regex2);
        printf("Postfix regex: %s\n", postfixRegex.c_str());

        // Call the Thompson construction function with the postfix regex
        NFA result = convertToNFA(regex);
        NFA result2 = regexToNFA(postfixRegex);

        // Assuming you have methods in the NFA class to print the NFA's states and transitions
        result2.printNFA(); // This method would print out the NFA's states and transitions
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
