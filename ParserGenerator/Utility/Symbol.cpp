#include "Symbol.h"

// Constructor to initialize the name
Symbol::Symbol(const std::string& name) : _name(name) {}

// Getter for name
const std::string& Symbol::getName() const {
    return _name;
}

// Setter for name
void Symbol::setName(const std::string& newName) {
    _name = newName;
}