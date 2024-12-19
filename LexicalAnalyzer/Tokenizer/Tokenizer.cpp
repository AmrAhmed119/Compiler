#include <sstream>
#include "Tokenizer.h"
#include <Algorithm>
#include "../../LexicalAnalyzer/Utility/Util.h"

std::unordered_set<std::shared_ptr<State>> readTransitionTable(const std::string &filePath);

std::unordered_set<std::shared_ptr<State>> readTransitionTable(const std::string &filePath) {
    std::unordered_set<std::shared_ptr<State>> states;
    std::unordered_map<int, std::shared_ptr<State>> stateMap; // Map state IDs to State objects
    std::ifstream file(filePath);

    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file " + filePath);
    }

    std::string line;
    int startState = 0;
    while (std::getline(file, line)) {
        // Skip empty lines or headers
        if (line.find("Start") != std::string::npos) {
            int pos = (int)line.find(':');
            startState = std::stoi(line.substr(pos + 1));
            continue;
        }

        if (line.empty() || line.find("State") != std::string::npos) {
            continue;
        }

        std::istringstream stream(line);
        int stateId, input, nextStateId;
        std::string tokenClass;

        // Parse the line
        stream >> stateId >> input >> nextStateId;
        std::getline(stream, tokenClass); // Read the rest of the line as the token class
        trimBlanksFromEnds(tokenClass);

        // Create or retrieve the current state
        if (stateMap.find(stateId) == stateMap.end()) {
            stateMap[stateId] = std::make_shared<State>();
        }
        auto currentState = stateMap[stateId];

        // Create or retrieve the next state
        if (stateMap.find(nextStateId) == stateMap.end()) {
            stateMap[nextStateId] = std::make_shared<State>();
        }
        auto nextState = stateMap[nextStateId];

        // Add transition
        currentState->addTransition(input, nextState);

        if (!tokenClass.empty()) {
            nextState->setTokenClass(tokenClass);
            nextState->setPriority(1);
        }

        // Add states to the set
        states.insert(currentState);
        states.insert(nextState);
    }
    stateMap[startState]->setStartingState(true);

    file.close();
    return states;
}

Tokenizer::Tokenizer(const std::string &programFilePath, const std::string &tableFilePath)
                    : _reader(programFilePath), _currentIndex(0) {

    // read states from the table file
    _states = readTransitionTable(tableFilePath);

    // set the starting state
    for (const auto& state : _states) {
        if (state->isStarting()) {
            _startingState = state;
            break;
        }
    }

    // read the first line
    _currentWords = _reader.readLine();
}

Tokenizer::~Tokenizer() {
    _reader.closeFile();
}

std::string Tokenizer::getNextToken() {

    std::string str = _currentWords.front();
    int strLen = (int)str.size();
    auto currentState = _startingState;
    int longestMatch = currentState->isAccepting() ? _currentIndex : -1;
    std::string tokenClass = currentState->isAccepting() ? currentState->getTokenClass() : "";

    for (int i = _currentIndex; i < strLen; i++) {
        char transition = str[i];
        auto transitions = currentState->getNextStates(transition);
        if (transitions.empty()) {
            break;
        }
        currentState = transitions[0];
        if (currentState->isAccepting()) {
            longestMatch = i;
            tokenClass = currentState->getTokenClass();
        }
    }

    if (longestMatch == -1) {
//        _currentWords.pop();
//        _currentIndex = 0;
        std::string x = _currentWords.front();
        std::string ans = "Error: No match found for {  ";
        ans += x[_currentIndex];
        ans += "  }";
        _currentIndex++;
        if (_currentIndex == strLen) {
            _currentWords.pop();
            _currentIndex = 0;
            if (_currentWords.empty() && _reader.hasMoreLines()) {
                _currentWords = _reader.readLine();
            }
        }
        return ans;
    }

    std::string token = str.substr(_currentIndex, longestMatch - _currentIndex + 1);
    _tokens.emplace_back(token, tokenClass);

    _currentIndex = longestMatch + 1;
    if (_currentIndex == strLen) {
        _currentWords.pop();
        _currentIndex = 0;
        if (_currentWords.empty() && _reader.hasMoreLines()) {
            _currentWords = _reader.readLine();
        }
    }

    return token;
}

bool Tokenizer::hasMoreTokens() {
    return !_currentWords.empty() || _reader.hasMoreLines();
}

std::vector<Token> Tokenizer::getTokens() {
    return _tokens;
}


