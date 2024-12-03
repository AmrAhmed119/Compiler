#include <iostream>
#include <stack>
#include <algorithm>
#include <fstream>
#include "NFA.h"
#include "process_tokens.h"



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
    if (op == '.')
        return 2;  // Concatenation precedence
    if (op == '|')
        return 1;  // Alternation precedence
    if (op == '*')
        return 3;  // Kleene star precedence (highest)
    return 0;  // For operands or unhandled cases
}

std::string infixToPostfix(const std::string &infix)
{
    std::stack<char> operators;  // Stack for operators
    std::string postfix;
    
    for (int i = 0; i < infix.size(); i++)
    {
        char c = infix[i];

        // Skip spaces
        if (c == ' ') {
            continue;
        }

        // if (c == '#'){
        //     c = '.';
        // }

        // Handle escaped characters (e.g., '\a')
        if (c == '\\') {
            postfix += infix[++i];
            continue;
        }

        // Handle opening parentheses
        if (c == '(') {
            operators.push(c);
        }
        // Handle closing parentheses
        else if (c == ')') {
            // Process operators until reaching '('
            while (!operators.empty() && operators.top() != '(') {
                postfix += operators.top();
                operators.pop();
            }
            operators.pop(); // Remove '('
        }
        // Handle the alternation operator '|'
        else if (c == '|') {
            while (!operators.empty() && precedence(operators.top()) >= precedence(c)) {
                postfix += operators.top();
                operators.pop();
            }
            operators.push(c);
        }
        // Handle Kleene star
        else if (c == '*') {
            postfix += c;  // Add '*' directly to the postfix expression
        }
        // Handle concatenation (.)
        else if (c == '.') {
            // Handle as a normal operator with precedence 2
            while (!operators.empty() && precedence(operators.top()) >= precedence(c)) {
                postfix += operators.top();
                operators.pop();
            }
            operators.push(c);
        }
        else {
            // Add the current operand to the postfix expression
            postfix += c;
        }
    }

    // Pop remaining operators from the stack
    while (!operators.empty()) {
        postfix += operators.top();
        operators.pop();
    }

    if(postfix[0] == ('+')){
        printf("yesssss\n");
    }

    return postfix;
}


std::string formatRegEx(std::string regex) {
    // Remove spaces from the input regex
    regex.erase(std::remove(regex.begin(), regex.end(), ' '), regex.end());
    
    std::string res;
    const std::vector<char> allOperators = {'|', '?', '+', '*', '^'};
    const std::vector<char> binaryOperators = {'^', '|'};
    
    for (size_t i = 0; i < regex.size(); i++) {
        // Check for the special case "\\"
        if (regex[i] == '\\' ) {
            res += "\\\\";// Add "\L" as is
            i++; 
            res += regex[i];         // Skip the 'L' character since it's part of "\L"
            continue;
        }
        
        char c1 = regex[i];
        
        // Replace dots ('.') with '#'
        if (c1 == '.') {
            res += "\\\\";
            
        }
            res+=c1;
        // Add a dot ('.', replaced by '#') for implicit concatenation
        if (i + 1 < regex.size()) {
            char c2 = regex[i + 1];

            if (c1 != '(' && c2 != ')' && 
                std::find(allOperators.begin(), allOperators.end(), c2) == allOperators.end() &&
                std::find(binaryOperators.begin(), binaryOperators.end(), c1) == binaryOperators.end()) {
                res += '.'; // Add a concatenation operator
            }
        }
    }

    return res;
}




// std::string infixToPostfix(const std::string& regex) {
//     std::string postfix;
//     std::stack<char> stack;

//     // Format the regex if necessary
//     std::string formattedRegEx = formatRegEx(regex);

//     for (char c : formattedRegEx) {
//         switch (c) {
//             case '(':
//                 stack.push(c);
//                 break;

//             case ')':
//                 while (!stack.empty() && stack.top() != '(') {
//                     postfix += stack.top();
//                     stack.pop();
//                 }
//                 stack.pop();  // Remove the '('
//                 break;

//             default:
//                 while (!stack.empty()) {
//                     char peekedChar = stack.top();
//                     int peekedCharPrecedence = getPrecedence(peekedChar);
//                     int currentCharPrecedence = getPrecedence(c);

//                     if (peekedCharPrecedence >= currentCharPrecedence) {
//                         postfix += stack.top();
//                         stack.pop();
//                     } else {
//                         break;
//                     }
//                 }
//                 stack.push(c);
//                 break;
//         }
//     }

//     // Pop all remaining operators from the stack
//     while (!stack.empty()) {
//         postfix += stack.top();
//         stack.pop();
//     }

//     return postfix;
// }




int main(){
    std::cout << "from nfa" << std::endl;

    // std::string regex;

    // std::ifstream file("/home/abdelrahman/Desktop/compiler/Compiler/LexicalAnalyzer/NFA/rules.txt"); // Open the file
    // if (!file.is_open()) {
    //     std::cerr << "Error: Could not open file." << std::endl;
    // }

    // std::string line;
    // int currentLine = 0;

    // // Read lines until the desired line number is reached
    // while (std::getline(file, line)) {
    //     currentLine++;
    //     // printf("line: %s\n", line.c_str());
    //     if (currentLine == 32) {
    //         regex = line; // Return the line when found
    //         // printf("regex: %s\n", regex.c_str());
    //         break;
    //     }
    // }


    std::string regex = "boolean";
    std::string regex2 = "(((((0)|(1)|(2)|(3)|(4)|(5)|(6)|(7)|(8)|(9))))+) | (((((0)|(1)|(2)|(3)|(4)|(5)|(6)|(7)|(8)|(9))))+ . (((((0)|(1)|(2)|(3)|(4)|(5)|(6)|(7)|(8)|(9))))+) (( \\L) | (E (((((0)|(1)|(2)|(3)|(4)|(5)|(6)|(7)|(8)|(9))))+))))";
 try
    {
        std::string ans = formatRegEx(regex2);
        printf("ans:  %s\n", ans.c_str() );

        std::string ans2 = formatRegEx(regex);
        printf("ans:  %s\n", ans.c_str() );

        // Convert infix regex to postfix
        std::string postfixRegex = infixToPostfix(ans2);
        printf("Postfix regex: %s\n", postfixRegex.c_str());

        std::string postfixRegex2 = infixToPostfix(ans);
        printf("Postfix regex2: %s\n", postfixRegex2.c_str());

        // Call the Thompson construction function with the postfix regex
        // NFA result2 = regexToNFA(postfixRegex);
        // std::shared_ptr<State> root = result2.startStates[0];
        //printNFA(root);

        // Assuming you have methods in the NFA class to print the NFA's states and transitions
        //result2.printNFA(); // This method would print out the NFA's states and transitions
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

}