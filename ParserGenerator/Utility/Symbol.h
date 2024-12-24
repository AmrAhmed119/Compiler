#ifndef PROJECT_SYMBOL_H
#define PROJECT_SYMBOL_H


#include <string>

class Symbol {
private:
    std::string _name; // Attribute to store the symbol name
    bool _isSync;

public:
    // Default constructor
    Symbol() : _isSync(false) {}

    // Destructor
    virtual ~Symbol() = default;

    // Constructor to initialize the name
    explicit Symbol(const std::string& name, bool isSync = false);

    // Getter for name
    const std::string& getName() const;

    // Setter for name
    void setName(const std::string& newName);
        
    // Getter for isSync
    bool getIsSync() const;

    // Setter for isSync
    void setIsSync(bool value);

    // Comparison operator
    bool operator<(const Symbol& other) const;
};


#endif //PROJECT_SYMBOL_H
