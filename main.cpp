#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <fstream>
#include <unistd.h>
#include "LexicalAnalyzer/NFA/process_tokens.h"
#include "LexicalAnalyzer/NFA/NFA.h"
#include <map>
#include <set>
#include <string.h>
#include <filesystem>


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

std::string getMainFileDirectory(const std::string& filename) {
    // Get the directory path of the current source file (main.cpp)
    std::filesystem::path currentFilePath = __FILE__;
    std::filesystem::path directoryPath = currentFilePath.parent_path();

    // Concatenate the directory path with the filename (e.g., "rules.txt")
    return (directoryPath / filename).string();
}

void printMap(const std::map<std::string, std::set<char>>& nameToCharSet) {
    for (const auto& pair : nameToCharSet) {
        std::cout << "Name: " << pair.first << " -> Characters: { ";
        for (char c : pair.second) {
            std::cout << c << " ";
        }
        std::cout << "}\n";
    }
}


void processFile(const std::string& filename, 
                 std::map<std::string, std::set<char>>& nameToCharSet, 
                 std::set<int>& allChars) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::string line;
    std::string name;
    std::set<char> charSet;
    int lineNumber = 1;

    // Special characters to exclude unless preceded by a backslash
    std::set<char> specialChars = {')', '(', '|', '*', '+', '='};

    // Read each pair of lines
    while (std::getline(file, line)) {
        if (lineNumber % 2 != 0) {
            // Odd lines: Treat this as a name (key)
            name = line;
        } else {
            // Even lines: Process the line to create a set of characters
            charSet.clear(); // Clear previous set
            bool isEscaped = false; // Flag to track if the previous character was a backslash

            for (size_t i = 0; i < line.size(); ++i) {
                char c = line[i];

                // If the current character is a backslash and the next one is a special char
                if (c == '\\' && i + 1 < line.size() && specialChars.count(line[i + 1])) {
                    // Mark as escaped
                    isEscaped = true;
                    continue; // Skip this iteration and the next special character will be added
                }

                // If the character is not special or it's escaped, add it
                if (!specialChars.count(c) || (isEscaped && specialChars.count(c))) {
                    if (!std::isspace(c)) { // Skip spaces if not needed
                        charSet.insert(c); // Insert valid characters into the set
                        allChars.insert(c); // Add the character to the global set
                    }
                }

                // Reset escape flag after processing
                isEscaped = false;
            }

            // Add the name and its corresponding set of chars to the map
            nameToCharSet[name] = charSet;
        }
        ++lineNumber;
    }

    file.close();
}


int main()
{
    // Example input regex: "a|b"
    std::string regex = "a|b";
    std::string regex2 = "((a|b)(a|b|0)*)";

    //std::cout << "test" << std::endl;

    std::map<std::string, std::set<char>> nameToCharSet;

    std::string filename = "rules.txt";
    std::string fullPath = getMainFileDirectory(filename);
    std::set<int> vocab;


    processFile(fullPath, nameToCharSet, vocab);
    // printMap(nameToCharSet);
    // std::cout << "\nCombined Character Set:\n";
    // for (const auto& c : vocab) {
    //     std::cout << c << " ";
    // }
    // std::cout << std::endl;


    try
    {
        // Convert infix regex to postfix
        std::string postfixRegex = infixToPostfix(regex);
        printf("Postfix regex: %s\n", postfixRegex.c_str());

        // Call the Thompson construction function with the postfix regex
        NFA result = convertToNFA(regex);
        NFA result2 = regexToNFA(postfixRegex);
        std::shared_ptr<State> root = result2.startStates[0];
        //printNFA(root);

        // Assuming you have methods in the NFA class to print the NFA's states and transitions
        //result2.printNFA(); // This method would print out the NFA's states and transitions
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

