#ifndef PROJECT_PARSERTABLE_H
#define PROJECT_PARSERTABLE_H


#include <string>
#include "../Utility/NonTerminal.h"

class ParserTable {
public:

    std::map<std::string,NonTerminal> nonTerminals;

    // Constructor
    ParserTable(std::map<std::string,NonTerminal>& nonTerminals);

    void parserTableCreator();


};


#endif //PROJECT_PARSERTABLE_H