#include "Production.h"

// Default constructor
Production::Production() = default;

// Constructor to initialize with a list of symbols
Production::Production(const std::vector<Symbol>& symbols) : _symbols(symbols) {}

// Getter for symbols
const std::vector<Symbol>& Production::getSymbols() const {
    return _symbols;
}

// Setter for symbols
void Production::setSymbols(const std::vector<Symbol>& newSymbols) {
    _symbols = newSymbols;
}

// Method to add a symbol to the list
void Production::addSymbol(const Symbol& symbol) {
    _symbols.push_back(symbol);
}

// Method to clear all symbols
void Production::clearSymbols() {
    _symbols.clear();
}
