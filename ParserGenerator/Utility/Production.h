#ifndef PROJECT_PRODUCTION_H
#define PROJECT_PRODUCTION_H


#include <vector>
#include "Symbol.h"

class Production {
private:
    // store symbols of the production
    // Ex: A -> aBc => _symbols = {a, B, c}
    std::vector<Symbol> _symbols;

public:
    // Default constructor
    Production();

    // Constructor to initialize with a list of symbols
    explicit Production(const std::vector<Symbol>& symbols);

    // Getter for symbols
    const std::vector<Symbol>& getSymbols() const;

    // Setter for symbols
    void setSymbols(const std::vector<Symbol>& newSymbols);

    // Method to add a symbol to the list
    void addSymbol(const Symbol& symbol);

    // Method to clear all symbols
    void clearSymbols();

};


#endif //PROJECT_PRODUCTION_H
