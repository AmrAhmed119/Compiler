#include <map>
#include <fstream>
#include "NFA.h"
#include "../../LexicalAnalyzer/NFA/process_tokens.h"
#include "../../LexicalAnalyzer/Utility/Util.h"

// Constructor for NFA
NFA::NFA() {
    states.clear();
    startState = nullptr;
    endStates.clear();
}

// Push an NFA onto the stack
void NFA::push(const NFA &nfa) {
    throw std::logic_error("Stack operations are not defined in this structure.");
}

// Pop an NFA from the stack
NFA NFA::pop() {
    throw std::logic_error("Stack operations are not defined in this structure.");
}

// Add a transition between states
void NFA::addTransition(std::shared_ptr<State> from, const int &symbol, std::shared_ptr<State> to) {
    if (!from || !to) {
        throw std::invalid_argument("From or to state is null.");
    }
    from->addTransition(symbol, to);
    states.insert(from);
    states.insert(to);
}

// Merge states from another NFA
void NFA::mergeStates(NFA &otherNFA) {
    for (auto &state : otherNFA.states) {
        states.insert(state);
    }
}

// Add an epsilon transition
void NFA::addEpsilonTransition(std::shared_ptr<State> from, std::shared_ptr<State> to) {
    addTransition(from, epsilon, to);  // Use '/l' to denote epsilon transitions
}

// Perform concatenation
void NFA::concatenate(NFA &nfa1) {
    for (auto &endState : endStates) {
        addEpsilonTransition(endState, nfa1.startState);
    }
    endStates = nfa1.endStates;
    mergeStates(nfa1);
}

// Perform OR operation
void NFA::orOp(NFA &nfa1) {
    auto newStart = std::make_shared<State>(false, -1);
    auto newEnd = std::make_shared<State>(false, -1);

    addEpsilonTransition(newStart, startState);
    addEpsilonTransition(newStart, nfa1.startState);

    for (auto &endState : endStates) {
        addEpsilonTransition(endState, newEnd);
    }

    for (auto &endState : nfa1.endStates) {
        addEpsilonTransition(endState, newEnd);
    }

    startState = newStart;
    endStates = {newEnd};
    mergeStates(nfa1);
    states.insert(newStart);
    states.insert(newEnd);
}

// Apply Kleene Star operation
void NFA::kleeneStar() {
    auto newStart = std::make_shared<State>(false, -1);
    auto newEnd = std::make_shared<State>(false, -1);

    addEpsilonTransition(newStart, startState);
    for (auto &endState : endStates) {
        addEpsilonTransition(endState, newEnd);
        addEpsilonTransition(endState, startState);
    }
    addEpsilonTransition(newStart, newEnd);

    startState = newStart;
    endStates = {newEnd};
    states.insert(newStart);
    states.insert(newEnd);
}

// Apply positive closure
void NFA::positiveClosure() {
    auto newStart = std::make_shared<State>(false, -1);
    auto newEnd = std::make_shared<State>(false, -1);

    addEpsilonTransition(newStart, startState);
    for (auto &endState : endStates) {
        addEpsilonTransition(endState, newEnd);
        addEpsilonTransition(endState, startState);
    }

    startState = newStart;
    endStates = {newEnd};
    states.insert(newStart);
    states.insert(newEnd);
}

// Process a single symbol
void NFA::processSymbol(const int &symbol) {
    auto start = std::make_shared<State>(false, -1);
    auto end = std::make_shared<State>(false, -1);
    addTransition(start, symbol, end);

    startState = start;
    endStates = {end};
    states.insert(start);
    states.insert(end);
}

// Compute epsilon closure
std::set<std::shared_ptr<State>> NFA::computeEpsilonClosure(const std::set<std::shared_ptr<State>> &inputStates) {
    std::set<std::shared_ptr<State>> closure = inputStates;
    std::stack<std::shared_ptr<State>> stack;

    for (auto &state : inputStates) {
        stack.push(state);
    }

    while (!stack.empty()) {
        auto current = stack.top();
        stack.pop();

        auto transitions = current->getTransitions();
        if (transitions.find(epsilon) != transitions.end()) {
            for (auto &nextState : transitions[epsilon]) {
                if (closure.find(nextState) == closure.end()) {
                    closure.insert(nextState);
                    stack.push(nextState);
                }
            }
        }
    }

    return closure;
}

void NFA::processFile(const std::string &filename,std::map<std::string, std::set<char>> &nameToCharSet,std::set<int> &allChars) {
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

std::shared_ptr<State> NFA::getStartState(std::ifstream& file) {
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
            print("Error: Could not process regex.", true);
            return nullptr;
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
        combinedNFA.addTransition(commonStartState, epsilon, nfa.startState);

        // Merge the states of each NFA into the combined NFA
        combinedNFA.mergeStates(nfa);
    }

    // Set the start state of the combined NFA
    combinedNFA.startState = commonStartState;
    combinedNFA.startState->setStartingState(true);
    return combinedNFA.startState;
}

