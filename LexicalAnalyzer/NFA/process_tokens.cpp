#include <iostream>
#include <stack>
#include <algorithm>
#include <fstream>
#include "NFA.h"
#include "process_tokens.h"
#include "NFA.h"
#include <stack>
#include <string>
#include <queue>


NFA buildNFAFromPostfix(const std::string &postfix) {
    std::stack<NFA> nfaStack;

    for (size_t i = 0; i < postfix.length(); ++i) {
        char ch = postfix[i];
        // if(nfaStack.size() != 0){
        //     std::shared_ptr<State> root = nfaStack.top().startStates[0];
        // printNFA(root);
        // }

        // Handle escaped characters
        if (ch == '\\') {
            if (i + 1 < postfix.length()) {
                char nextChar = postfix[++i]; // Get the next character
                if (nextChar == 'L') {
                    // Handle epsilon
                    NFA epsilonNFA;
                    epsilonNFA.processSymbol('/l');
                    nfaStack.push(epsilonNFA);
                } else {
                    // Handle regular escaped character
                    NFA charNFA;
                    charNFA.processSymbol(nextChar);
                    nfaStack.push(charNFA);
                }
            } else {
                throw std::runtime_error("Invalid escape sequence in postfix regex");
            }
        } else if (ch == '|') {
            // OR operation
            if (nfaStack.size() < 2) throw std::runtime_error("Invalid postfix expression for '|'");
            NFA nfa2 = nfaStack.top(); nfaStack.pop();
            NFA nfa1 = nfaStack.top(); nfaStack.pop();
            nfa1.orOp(nfa2);
            nfaStack.push(nfa1);
        } else if (ch == '*') {
            // Kleene star
            if (nfaStack.empty()) throw std::runtime_error("Invalid postfix expression for '*'");
            NFA nfa = nfaStack.top(); nfaStack.pop();
            nfa.kleeneStar();
            nfaStack.push(nfa);
        } else if (ch == '+') {
            // Positive closure
            if (nfaStack.empty()) throw std::runtime_error("Invalid postfix expression for '+'");
            NFA nfa = nfaStack.top(); nfaStack.pop();
            nfa.positiveClosure();
            nfaStack.push(nfa);
        } else if (ch == '.') {
            // Concatenation
            if (nfaStack.size() < 2) throw std::runtime_error("Invalid postfix expression for '.'");
            NFA nfa2 = nfaStack.top(); nfaStack.pop();
            NFA nfa1 = nfaStack.top(); nfaStack.pop();
            nfa1.concatenate(nfa2);
            nfaStack.push(nfa1);
        } else {
            // Operand (character)
            NFA charNFA;
            charNFA.processSymbol(ch);
            nfaStack.push(charNFA);
        }
    }

    // At the end, there should be exactly one NFA on the stack
    if (nfaStack.size() != 1) throw std::runtime_error("Invalid postfix expression");
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
            i++;
            postfix += infix[i];
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
            //res += regex[i];         // Skip the 'L' character since it's part of "\L"
            //continue;
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
