
#include "Tokenizer.h"

Tokenizer::Tokenizer(const std::string &filePath, const std::unordered_set<std::shared_ptr<State>> &states)
                    : _reader(filePath), _states(states), _currentIndex(0) {
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
    int strLen = str.size();
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
        _currentWords.pop();
        std::string ans = "Error: No match found for {  " + str.substr(_currentIndex) + "  }";
        _currentIndex = 0;
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


