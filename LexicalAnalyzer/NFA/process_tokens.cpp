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

    std::string regex;

    std::ifstream file("/home/abdelrahman/Desktop/compiler/Compiler/LexicalAnalyzer/NFA/rules.txt"); // Open the file
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file." << std::endl;
    }

    std::string line;
    int currentLine = 0;

    // Read lines until the desired line number is reached
    while (std::getline(file, line)) {
        currentLine++;
        // printf("line: %s\n", line.c_str());
        if (currentLine == 2) {
            regex = line; // Return the line when found
            // printf("regex: %s\n", regex.c_str());
            break;
        }
    }


    // std::string regex = "(\\+\\+)  ";
    std::string regex2 = "(((((0)|(1)|(2))))+ . (((((0))))+) (( \\L) | (E (((((0))))+))))";
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

        NFA result = buildNFAFromPostfix(postfixRegex);

        std::shared_ptr<State> root = result.startState;
        printNFA(root);

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