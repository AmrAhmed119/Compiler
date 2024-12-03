#ifndef REGEX_NFA_H
#define REGEX_NFA_H

#include <iostream>
#include <stack>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include "NFA.h"
#include "process_tokens.h"

// Function to convert a postfix regular expression to an NFA
NFA regexToNFA(const std::string& postfix);

// Function to determine operator precedence
int precedence(char op);

// Function to convert an infix regular expression to postfix notation
std::string infixToPostfix(const std::string& infix);

// Function to format a regular expression (adds concatenation operators and handles escape sequences)
std::string formatRegEx(std::string regex);

#endif // REGEX_NFA_H
