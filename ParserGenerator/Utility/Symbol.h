#ifndef PROJECT_SYMBOL_H
#define PROJECT_SYMBOL_H

#include <string>

class Symbol
{
private:
    std::string _name; // Attribute to store the symbol name
    bool isSync; // Attribute to store whether the terminal is sync

public:
    // Default constructor
    Symbol() : _isSync(false) {}

    // Destructor
    virtual ~Symbol() = default;

    // Constructor to initialize the name
    explicit Symbol(const std::string &name);

    // Constructor to initialize the name and isSync
    Symbol(const std::string& name, bool isSync);


    // Getter for name
    const std::string &getName() const;

    // Getter for isSync
    bool getIsSync() const;


    void setName(const std::string &newName);

    // Comparison operator
    bool operator<(const Symbol &other) const;
};

#endif // PROJECT_SYMBOL_H
