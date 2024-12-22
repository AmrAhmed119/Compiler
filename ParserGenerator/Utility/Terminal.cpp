#include "Terminal.h"

// Constructor to initialize name and isEpsilon
Terminal::Terminal(const std::string& name, bool isEpsilon) : Symbol(name), isEpsilon(isEpsilon) {}

// Getter for isEpsilon
bool Terminal::getIsEpsilon() const {
    return isEpsilon;
}

// Setter for isEpsilon
void Terminal::setIsEpsilon(bool value) {
    isEpsilon = value;
}