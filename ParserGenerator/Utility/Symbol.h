#ifndef PROJECT_SYMBOL_H
#define PROJECT_SYMBOL_H


#include <string>

class Symbol {
private:
    std::string _name; // Attribute to store the symbol name

public:
    // Default constructor
    Symbol() = default;

    // Destructor
    virtual ~Symbol() = default;

    // Constructor to initialize the name
    explicit Symbol(const std::string& name);

    // Getter for name
    const std::string& getName() const;

    // Setter for name
    void setName(const std::string& newName);

    // Comparison operator
    bool operator<(const Symbol& other) const;
};


#endif //PROJECT_SYMBOL_H
