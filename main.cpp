#include <iostream>
#include <string>
#include <queue>
#include <fstream>
#include <map>
#include <set>
#include <filesystem>
#include "LexicalAnalyzer/RulesParser/RulesParser.h"
#include "LexicalAnalyzer/NFA/process_tokens.h"
#include "LexicalAnalyzer/NFA/NFA.h"
#include "LexicalAnalyzer/DFA/DFA.h"
#include "LexicalAnalyzer/Tokenizer/Tokenizer.h"

const std::string rulesPath = "../LexicalAnalyzer/Inputs/rules.txt";
const std::string intermediatePath = "../LexicalAnalyzer/Inputs/rules.txt_regularExpressions.txt";
const std::string programPath = "../LexicalAnalyzer/Inputs/program.txt";

void processFile(const std::string &filename,
                 std::map<std::string, std::set<char>> &nameToCharSet,
                 std::set<int> &allChars) {
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
            charSet.clear();        // Clear previous set
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
                    if (!std::isspace(c)) {                       // Skip spaces if not needed
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

int parseRules() {
    RulesParser rulesParser(rulesPath);
    int statusCode = rulesParser.parseFile();
    if (statusCode == -1) {
        std::cerr << "Error: Parsing failed" << std::endl;
        return -1;
    }
    return 0;
}

DFA *performDFA(NFA nfa, std::set<int> vocab) {
    DFA *dfa = new DFA();
    dfa->_epsCode = '/l';
    dfa->createDFA(nfa.startState, vocab);
    return dfa;
}

void performTokenization(std::string filePath, std::unordered_set<std::shared_ptr<State>> states) {
    Tokenizer tokenizer(filePath, states);
    std::ofstream outputFile("../LexicalAnalyzer/Outputs/tokens.txt", std::ios::out);
    if (!outputFile) {
        std::cerr << "Error: Unable to open output file \"tokens\"." << std::endl;
        return;
    }

    outputFile << "Tokenization results for the input program:\n\n";
    while (tokenizer.hasMoreTokens()) {
        std::string token = tokenizer.getNextToken();
        outputFile << token << "\n";
    }

    outputFile << "\n\nTokenization results in detail:\n\n";
    for (const auto &token: tokenizer.getTokens()) {
        outputFile << std::left << std::setw(20) << ("Token: " + token.getValue())
                   << std::setw(20) << ("Class: " + token.getType()) << "\n";
    }

    outputFile << "\n\nSymbol Table:\n\n";
    std::vector<Token> symbolTable = tokenizer.getTokens();
    std::set<std::string> uniqueIdentifiers;
    for (const auto &token: symbolTable) {
        if (token.getType() == "id" && uniqueIdentifiers.find(token.getValue()) == uniqueIdentifiers.end()) {
            uniqueIdentifiers.insert(token.getValue());
            outputFile << std::left << std::setw(40) << ("variable-name: " + token.getValue())
                       << std::setw(20) << ("Class: " + token.getType()) << "\n";
        }
    }

    for (const auto &token: symbolTable) {

    }

    outputFile.close();
}

void printTransitionTable(std::unordered_set<std::shared_ptr<State>> states) {
    std::ofstream outputFile("../LexicalAnalyzer/Outputs/transitionTable.txt", std::ios::out);
    if (!outputFile) {
        std::cerr << "Error: Unable to open output file \"transition_table\"." << std::endl;
        return;
    }

    // create mapping for each state to its name
    std::map<std::shared_ptr<State>, int> stateToName;
    int i = 0;
    for (const auto &state: states) {
        stateToName[state] = i++;
    }

    outputFile << "Transition table for the minimized DFA:\n\n";
    outputFile << std::left << std::setw(20) << "State" << std::setw(20) << "Transition" << std::setw(20)
               << "Next State"
               << std::setw(20) << "Token Class" << "\n";
    for (const auto &state: states) {
        for (const auto &transition: state->getTransitions()) {
            outputFile << std::left << std::setw(20) << stateToName[state] << std::setw(20) << transition.first
                       << std::setw(20) << stateToName[transition.second[0]] << std::setw(20)
                       << transition.second[0]->getTokenClass() << "\n";
        }
    }
}

int main() {

    std::cout << "STEP1 : Parsing rules..." << std::endl;
    parseRules();

    std::cout << "STEP2 : Starting NFA creation..." << std::endl;
    std::ifstream file(intermediatePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file." << std::endl;
        return 1;
    }

    std::set<int> vocab;
    std::map<std::string, std::set<char>> nameToCharSet;
    processFile(intermediatePath, nameToCharSet, vocab);

    std::string line;
    int currentLine = 0;
    int priority = 0;
    std::vector<NFA> nfaList;

    std::string tokenClass;
    while (std::getline(file, line)) {
        currentLine++;

        // Odd lines contain token class names (skip them for now)
        if (currentLine % 2 != 0) {
            tokenClass = line;
            continue;
        }

        // Even lines contain regex expressions
        std::string regex = line;
        try {
            // Format the regex
            std::string formattedRegex = formatRegEx(regex);

            // Convert to postfix notation
            std::string postfixRegex = infixToPostfix(formattedRegex);

            // Build the NFA from the postfix regex
            NFA nfa = buildNFAFromPostfix(postfixRegex);

            // Set priorities for accepting states
            for (auto &endState: nfa.endStates) {
                endState->setPriority(priority);
                endState->setTokenClass(tokenClass);
                priority++;
            }

            // Add the NFA to the list
            nfaList.push_back(nfa);
        }
        catch (const std::exception &e) {
            std::cerr << "Error processing regex: " << regex << "\n"
                      << e.what() << std::endl;
            return 1;
        }
    }

    file.close();

    // Combine all NFAs into one
    NFA combinedNFA;

    // Create a new common start state
    auto commonStartState = std::make_shared<State>(true);
    combinedNFA.states.insert(commonStartState);

    for (auto &nfa: nfaList) {
        // Add epsilon transitions from the common start state to each NFA's start state
        combinedNFA.addTransition(commonStartState, '/l', nfa.startState);

        // Merge the states of each NFA into the combined NFA
        combinedNFA.mergeStates(nfa);
    }

    // Set the start state of the combined NFA
    combinedNFA.startState = commonStartState;
    combinedNFA.startState->setStartingState(true);


    std::cout << "STEP3 : Starting DFA creation..." << std::endl;
    DFA *dfa = performDFA(combinedNFA, vocab);

    std::cout << "STEP4 : Minimizing DFA..." << std::endl;
    std::unordered_set<std::shared_ptr<State>> minimizedDFA = dfa->minimizeDFA();

    printTransitionTable(minimizedDFA);

    std::cout << "STEP5 : Starting Token extraction..." << std::endl;
    performTokenization(programPath, minimizedDFA);
}

