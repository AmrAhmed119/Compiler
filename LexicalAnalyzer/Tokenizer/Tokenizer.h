
#ifndef PROJECT_TOKENIZER_H
#define PROJECT_TOKENIZER_H


#include <memory>
#include <unordered_set>
#include "FileReader.h"
#include "../Utility/State.h"
#include "Token.h"

class Tokenizer {
    FileReader _reader;
    std::unordered_set<std::shared_ptr<State>> _states;
    std::shared_ptr<State> _startingState;
    std::queue<std::string> _currentWords;
    std::vector<Token> _tokens;
    int _currentIndex;

public:
    explicit Tokenizer(const std::string& programFilePath, const std::string& tableFilePath);
    ~Tokenizer();
    std::string getNextToken();
    bool hasMoreTokens();
    std::vector<Token> getTokens();
};


#endif //PROJECT_TOKENIZER_H
