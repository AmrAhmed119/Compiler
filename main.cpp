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
#include "LexicalAnalyzer/DFA/DFA.h"
#include "LexicalAnalyzer/Tokenizer/Tokenizer.h"

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

std::string getMainFileDirectory(const std::string &filename)
{
    // Get the directory path of the current source file (main.cpp)
    std::filesystem::path currentFilePath = __FILE__;
    std::filesystem::path directoryPath = currentFilePath.parent_path();

    // Concatenate the directory path with the filename (e.g., "rules.txt")
    return (directoryPath / filename).string();
}

void printMap(const std::map<std::string, std::set<char>> &nameToCharSet)
{
    for (const auto &pair : nameToCharSet)
    {
        std::cout << "Name: " << pair.first << " -> Characters: { ";
        for (char c : pair.second)
        {
            std::cout << c << " ";
        }
        std::cout << "}\n";
    }
}

void processFile(const std::string &filename,
                 std::map<std::string, std::set<char>> &nameToCharSet,
                 std::set<int> &allChars)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
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
    while (std::getline(file, line))
    {
        if (lineNumber % 2 != 0)
        {
            // Odd lines: Treat this as a name (key)
            name = line;
        }
        else
        {
            // Even lines: Process the line to create a set of characters
            charSet.clear();        // Clear previous set
            bool isEscaped = false; // Flag to track if the previous character was a backslash

            for (size_t i = 0; i < line.size(); ++i)
            {
                char c = line[i];

                // If the current character is a backslash and the next one is a special char
                if (c == '\\' && i + 1 < line.size() && specialChars.count(line[i + 1]))
                {
                    // Mark as escaped
                    isEscaped = true;
                    continue; // Skip this iteration and the next special character will be added
                }

                // If the character is not special or it's escaped, add it
                if (!specialChars.count(c) || (isEscaped && specialChars.count(c)))
                {
                    if (!std::isspace(c))
                    {                       // Skip spaces if not needed
                        charSet.insert(c);  // Insert valid characters into the set
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

void getTokenizerResults(std::string filePath, std::unordered_set<std::shared_ptr<State>> states)
{
    std::cout<<"Start Tokenizer\n";
    Tokenizer tokenizer(filePath, states);
    while (tokenizer.hasMoreTokens())
    {
        std::cout << tokenizer.getNextToken() << "\n";
    }

    std::cout << "----------------Tokens---------------" << "\n";

    for (const auto &token : tokenizer.getTokens())
    {
        std::cout << "Token: " << token.getValue() << " Class: " << token.getType() << "\n";
    }
}

int main()
{
    std::cout << "Starting NFA creation..." << std::endl;

    std::string path = getMainFileDirectory("/home/ahmed/level4term1/compilers/Project/Compiler/LexicalAnalyzer/Inputs/rules.txt");

    // File path to the rules file
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file." << std::endl;
        return 1;
    }

    std::set<int> vocab;
    std::map<std::string, std::set<char>> nameToCharSet;
    processFile(path, nameToCharSet, vocab);

    std::string line;
    int currentLine = 0;
    int priority = 0;         // Priority counter for accepting states
    std::vector<NFA> nfaList; // List to hold individual NFAs

    while (std::getline(file, line))
    {
        currentLine++;

        // Odd lines contain token class names (skip them for now)
        if (currentLine % 2 != 0)
        {
            std::cout << "Token class: " << line << std::endl;
            continue;
        }

        // Even lines contain regex expressions
        std::string regex = line;
        try
        {
            // Format the regex
            std::string formattedRegex = formatRegEx(regex);

            // Convert to postfix notation
            std::string postfixRegex = infixToPostfix(formattedRegex);

            // Build the NFA from the postfix regex
            NFA nfa = buildNFAFromPostfix(postfixRegex);

            // Set priorities for accepting states
            for (auto &endState : nfa.endStates)
            {
                endState->setPriority(priority); // Assign priority to accepting states
                // printf("End state: %p, Priority: %d\n", endState.get(), priority);
                endState->setTokenClass(line); // Assign the token class to accepting states
                // printf("End state: %p, Token class: %s\n", endState.get(), line.c_str());
            }
            priority++; // Increment priority for the next regex

            // Add the NFA to the list
            nfaList.push_back(nfa);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error processing regex: " << regex << "\n"
                      << e.what() << std::endl;
            return 1;
        }
    }

    file.close();

    // Combine all NFAs into one
    NFA combinedNFA;

    // Create a new common start state
    auto commonStartState = std::make_shared<State>(true); // Mark as starting state
    combinedNFA.states.insert(commonStartState);           // Add to the combined NFA

    for (auto &nfa : nfaList)
    {
        // Add epsilon transitions from the common start state to each NFA's start state
        combinedNFA.addTransition(commonStartState, '/l', nfa.startState); // Assuming -1 represents epsilon

        // Merge the states of each NFA into the combined NFA
        combinedNFA.mergeStates(nfa);
    }

    // Set the start state of the combined NFA
    combinedNFA.startState = commonStartState;
    // printNFA(combinedNFA.startState);

    std::cout<<"NFA Created\n";
    DFA *dfa = new DFA();
    dfa->_epsCode = '/l';
    dfa->createDFA(combinedNFA.startState, vocab);
    std::cout<<"DFA created\n";
    std::unordered_set<std::shared_ptr<State>> minimizedDFA = dfa->minimizeDFA();
    std::cout<<"DFA minimized with size = "<<minimizedDFA.size()<<"\n";
    getTokenizerResults("/home/ahmed/level4term1/compilers/Project/Compiler/LexicalAnalyzer/Inputs/program2.txt", minimizedDFA);
}
