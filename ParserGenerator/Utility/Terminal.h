#ifndef PROJECT_TERMINAL_H
#define PROJECT_TERMINAL_H


#include "Symbol.h"

class Terminal : public Symbol {
private:
    bool isEpsilon; // Attribute to store whether the terminal is epsilon

public:

    // Constructor to initialize isEpsilon
    explicit Terminal(const std::string& name, bool isEpsilon);

    // Getter for isEpsilon
    bool getIsEpsilon() const;

    // Setter for isEpsilon
    void setIsEpsilon(bool value);

    // Comparison operator
    bool operator<(const Terminal& other) const;
};

#endif //PROJECT_TERMINAL_H
