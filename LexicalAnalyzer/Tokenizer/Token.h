
#ifndef PROJECT_TOKEN_H
#define PROJECT_TOKEN_H


#include <string>

class Token {
    std::string _value;
    std::string _type;

public:
    Token(std::string value, std::string type) : _value(value), _type(type) {}

    std::string getValue() const {
        return _value;
    }

    std::string getType() const {
        return _type;
    }
};


#endif //PROJECT_TOKEN_H
